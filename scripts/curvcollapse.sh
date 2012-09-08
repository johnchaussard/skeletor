skelpar3d $1 9 -1 _curves.pgm
~/Progs/collapse/bin/pgm2cca $1 _cca.cc
seuil $1 256 _vide.pgm
~/Progs/collapse/bin/pgm2cca _vide.pgm _vide.cc
rm -rf _vide.pgm
~/Progs/collapse/bin/cca_pgm_keep_curve_extremities _curves.pgm _points.cc _vide.cc
rm -rf _curves.pgm _vide.cc
~/Progs/collapse/bin/cca_pardircollapse _cca.cc $2 no 0 NULL _points.cc
rm -rf _cca.cc _inhib.cc _points.cc
