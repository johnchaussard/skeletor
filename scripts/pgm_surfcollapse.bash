#!/bin/bash


PROG_ROOT="/home/mockba/Progs/"
PROG_PGM_CURV_COLLAPSE=$PROG_ROOT"collapse/bin/pgm_curvskel"
PROG_PGM_SURF_COLLAPSE=$PROG_ROOT"collapse/bin/pgm_surfskel"

getdfrompic()
{
	d=`showheader $1 | grep "depth" | sed s/".*depth = \([0-9]*\).*"/"\1"/g`
	if [ "$d" = "" ]
	then
		echo "1"
	else
		echo "$d"
	fi
}


if [ ! $# -eq 2 ]
then
	echo "usage: " $0 "input_pgm output_pgm"
	exit 1
fi


trash=`$PROG_PGM_SURF_COLLAPSE $1 $2`
`dist $2 3 $2"_skelcubcomp_d"`
`inverse $2"_skelcubcomp_d" $2"_skelcubcomp_d"`
d=`getdfrompic $1`
trash=`skeleton $1 $2"_skelcubcomp_d" 26 $2 $2`

rm $2"_skelcubcomp_d"
