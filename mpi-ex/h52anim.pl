#!/usr/bin/perl

use strict;
use warnings;

use threads;
use Thread::Queue;

sub make_plot {
  my ( $h5file, $var, $m, $n, $it, $pngfile ) = @_;

  # start the gnuplot command
  open(GNUPLOT, "|-", "gnuplot") or die("cannot start gnuplot");

  # start a new plot, define styles
  print(GNUPLOT <<"EOT");
#set terminal pngcairo font "Arial,20" size 900,300 

set pm3d interpolate 1,1
unset key
set palette defined (  0 "blue", 19 "cyan", 20 "green", 21 "yellow", 40 "red")

set cbrange [-1.001:1.001]

#set hidden3d

set term png size 1000,600
set view 60,30
set view equal
set zrange [-1.001:1.001]
set output '$pngfile'
splot '-' w pm3d
EOT

  # start reading data
  open(H5DATA, "-|", "h5dump", "-d", $var, "-c", "$m,$n,1",
         "-s", "0,0,$it", $h5file)
    or die("error while reading dataset", $!);

  # skip until "DATA {"
  while (my $line = <H5DATA>) {
    last if $line =~ m/DATA \{/;
  }

  for my $i (1..$m) {

    my @values;

    while (@values < $n) {
      my $line = <H5DATA>;
      chomp $line;
      $line =~ s/^.*: *//;
      push(@values, split(/,/, $line));
    }

    for my $j (1..$n) {
      my $value = shift @values;
      printf(GNUPLOT "%4d %4d %8.3f\n", $i, $j, $value);
    }
    print(GNUPLOT "\n");
  }

  close(H5DATA);

  # mark the end of the plot data
  print(GNUPLOT "e\n");

  # close output file
  print(GNUPLOT "set output\n");

  # quit gnuplot
  close(GNUPLOT);

  return;
}

# ------------------------------------------------------------------------
# main
# ------------------------------------------------------------------------

my ($h5file, $var, $np);

$np = 1; # only 1 process, default value

while (@ARGV > 2) {
  if ($ARGV[0] =~ m/^-/) {
    my ($option, $value) = splice(@ARGV, 0, 2);
    $np = $value if ($option eq '-p');
  }
}

($h5file, $var) = @ARGV;

unless (defined $h5file) {
  printf("*** error: no file specified.\n",
         "           usage:\n",
         "              $0 <hdf file> <variable name>\n");
  exit(1);
}

unless (defined $var) {
  printf("*** error: no variable specified.\n",
         "           usage:\n",
         "              $0 <hdf file> <variable name>\n");
  exit(1);
}

my $outname = $h5file . ".gif";

# read the HDF5 metadata
my ($m, $n, $nt);

open(H5META, "-|", "h5dump", "-d", $var, "-H", $h5file)
  or die("error while reading dataset metadata", $!);

while(my $line = <H5META>) {

  chomp $line;
  if ($line =~ m/DATASPACE *SIMPLE *\{ *\( *(\d+), *(\d+), *(\d+)/) {
    $m = $1;
    $n = $2;
    $nt = $3;
  }
}

close(H5META);

unless (defined $m && defined $n && defined $nt) {
  printf("*** error: could not determine dataset metadata.\n");
  exit(1);
}

my $q = Thread::Queue -> new();

my @images;

for my $it (0..$nt - 1) {

  my $pngfile = "tmp-$it.png";

  my $args = [ $h5file, $var, $m, $n, $it, $pngfile ];

  $q -> enqueue($args);

  push(@images, $pngfile);
}

# not supported on perl 5.14.1. # $q -> end();


my @threads;

# fire up $np workers
for my $ip (1 .. $np) {
  my $thr = threads -> create(
    sub {
      while ( defined ( my $args = $q -> dequeue_nb() ) ) {
        print("[INFO] Plotting step $$args[4]");
        print("\n");
        make_plot(@{ $args });
      }
      return;
    } # end worker sub
  );
  push(@threads, $thr);
}

# wait for all threads to finish
for my $thr (@threads) {
  $thr -> join();
  print("thread joined\n");
}

# done plotting; stack all images in one animated gif

# uses the "ImageMagick" command "convert"

# put a 10/100 = 0.1s delay between frames
system("convert", "-delay", "10", @images, "-loop", "0", $outname);

for my $image (@images) {
  unlink($image);
}
