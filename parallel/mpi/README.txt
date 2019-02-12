
WAVE - simulation of 2D-wave propagation in time, using parallel computation

credits: Lykle Voort (SURFsara)

REQUIRED SOFTWARE

* MPI library
* HDF5
* make
* gcc

COMPILING

To compile the program:

build the wave simulation program by running

  make

in this directory.

If you want to use OpenMP, add the flag "-fopenmp" to both the CFLAGS and
the LDFLAGS.



RUNNING wave

To run wave, use the run_wave job script; the shell script sets the options
needed for the simulation; however, feel free to play with the settings
and observe the effects of the time step on the stability of the solution,
the compute time, and explore the effects of using more cores.


FURTHER POST-PROCESSING

The results are written to an HDF5 file, this is a standard binary format for
storing simulation results. Third-party programs, such as Matlab, can
read and easily visualise the results.






