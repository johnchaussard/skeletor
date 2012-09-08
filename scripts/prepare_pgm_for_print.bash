#!/bin/bash

if [ ! $# -eq 3 ]
then
	echo "usage: " $0 "main_object skeleton output"
	exit 1
fi

~/Progs/collapse/bin/div2 $1 4 $3
sub $1 $3 $3
sub $3 $2 $3
inverse $1 temp_inv
add $3 temp_inv $3
rm temp_inv
