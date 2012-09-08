#Performs surfacic and curvilinear collapse of shape $1 (pgm)
distc $1 6 _d
echo "Distance dilat"
~/Progs/collapse/bin/distance_dilat _d _dd
rm -rf _d
~/Progs/collapse/bin/pgm2cca $1 _cca
echo "Surf collapse"
~/Progs/collapse/bin/cca_surfcollapse _cca _surf _dd
echo "Curv collapse"
~/Progs/collapse/bin/cca_curvcollapse _cca _curv _dd
rm -rf _dd
echo "Mix collapse"
~/Progs/collapse/bin/cca_filter_surface _surf _surf 20
~/Progs/collapse/bin/cca_or _curv _surf _mix
~/Progs/collapse/bin/cca_collapse _cca _mixcoll _mix
rm -rf _mix
echo "Decomposition"
~/Progs/collapse/bin/cca_keepnonregularedges _mixcoll _edges
~/Progs/collapse/bin/cca_keepfaces _mixcoll _faces
~/Progs/collapse/bin/cca_makecomplex _faces _faces
~/Progs/collapse/bin/cca_sub _mixcoll _faces _curves
~/Progs/collapse/bin/cca_makecomplex _curves _curves
~/Progs/collapse/bin/cca_decompose_surfaces _faces _edges surf_decomp fusion NULL keep NULL
~/Progs/collapse/bin/cca_decompose_curve _curves curv_decomp.iv
rm -rf _edges _faces _curves

