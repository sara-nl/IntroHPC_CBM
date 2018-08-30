#!/bin/bash

echo "Hi, I'm your first script."
echo

lscpu --help > cpu.log
lscpu >> cpu.log

echo "I've left something for you."
echo "Ciao"

