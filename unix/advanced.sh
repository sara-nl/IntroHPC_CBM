#!/bin/bash
# A more advanced bash script.
# <- this is a comment and everything that follow is ignored

args=$# # Number of args passed.

if [ $args = 0 ]
then
 echo "This script needs a number as argument input."
 echo "Ex: ./advanced.sh 10"
 exit
fi

num=$1 # First argument passed.

echo "The input number is:" $1 
echo

cd src

gcc -o ../fact.exe fact.c 1> compile.log 2> compile.err # Redirect stdout and stderr to files

cd ..

./fact.exe $num | tee fact.log # Pipe output into command

echo
echo "Done"
