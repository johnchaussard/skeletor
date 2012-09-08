#!/bin/bash

source ~/Progs/collapse_dev/scripts/commons.bash

#Need an input file, and a skel.cc and axis.cc, d, dd, dd_d

echo "Computing various distance maps"
#~/Progs/collapse_dev/bin/cca_decompose_volume $1 skel.cc axis.cc skelabel.pgm
#seuil skelabel.pgm 1 s >> ici, utiliser plutot un squelette non filtre...
#dist s 6 ds
#sub dd d dd_d
#max dd_d ds dw
#long2byte dw 0 dw
#rm -rf s

max=`max1 skelabel.pgm`
let max=$max+1

echo "Flooding process"
watershedMeyer2lab skelabel.pgm dw $1 26 vol_decomp.pgm

echo "Second flooding process"
seuil dd_d 1 noncenter
seuil vol_decomp.pgm $max vols
sub vol_decomp.pgm vols vol_decomp2.pgm
sub vol_decomp2.pgm noncenter skelabelf.pgm
rm -rf noncenter vols
watershedMeyer2lab skelabelf.pgm dw $1 26 vol_decomp_filter.pgm


echo "Fight for the quarrel zones"
seuil vol_decomp_filter.pgm $max s
sub vol_decomp_filter.pgm s vol_decomp_filter2.pgm
~/Progs/collapse_dev/bin/pgmlabel_intersect vol_decomp2.pgm vol_decomp_filter2.pgm fight.pgm
rm -rf vol_decomp2.pgm vol_decomp_filter2.pgm
long2byte dd 0 dd
watershedMeyer2lab fight.pgm dd $1 26 vol_decomp_fight.pgm





