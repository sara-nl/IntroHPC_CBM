#!/bin/bash
# A more advanced bash script.
# <- this is a comment and everything that follow is ignored

echo "Hi, I'm your second script."
echo

NUM_START=10

if [ $# = 0 ]
then
 echo "This script needs a number as argument input."
 echo "Ex: ./advanced_script.sh 1"
 exit
fi

echo "The number you gave me is:" $1

mkdir result

cd result
echo "$NUM_START / $1" | bc > $USER.log 2> $USER.err
cd ..


echo "Done"
