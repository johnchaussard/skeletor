#Input pgm image
#Output cc image
echo "Computing surfaces to keep"
distc $1 6 _dist.pgm
~/Progs/collapse/bin/distance_dilat _dist.pgm _dist.pgm
~/Progs/collapse/bin/pgm2cca $1 _cca.cc
~/Progs/collapse/bin/cca_pardircollapse _cca.cc _faces.cc _dist.pgm NULL
~/Progs/collapse/bin/cca_keepfaces _faces.cc _faces.cc
~/Progs/collapse/bin/cca_makecomplex _faces.cc _faces.cc
rm -rf _dist.pgm
echo "Computing end points to keep"
skelpar3d $1 9 -1 _curves.pgm
~/Progs/collapse/bin/cca_pgm_keep_curve_extremities _curves.pgm _points.cc _faces.cc
rm -rf _curves.pgm
~/Progs/collapse/bin/cca_or _points.cc _faces.cc _inhib.cc
rm -rf _points.cc _faces.cc 
echo "Final collapse"
~/Progs/collapse/bin/cca_collapse _cca.cc $2 _inhib.cc
rm -rf _cca.cc _inhib.cc

