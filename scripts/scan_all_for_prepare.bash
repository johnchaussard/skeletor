#!/bin/bash

if [ ! $# -eq 2 ]
then
	echo "usage: " $0 "shape skeleton_directory"
	exit 1
fi

List="$(ls $2/*.pgm)"

~/Progs/collapse/bin/div2 $1 4 temp_script
sub $1 temp_script temp_script
inverse $1 temp_inv


for i in ${List[*]};
do
	name=`echo $i | sed s/".pgm"//g`
	sub temp_script $i temp_res.pgm
	add temp_res.pgm temp_inv temp_res.pgm
	convert temp_res.pgm $name.eps
done


rm temp_inv temp_res.pgm temp_script
