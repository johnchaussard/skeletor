#For IWCIA, this script computes various skeletons of objects
#Just get in the directory of the object, and run with pgm object's name without extension!!
~/Progs/collapse/bin/pgm2iv $1.pgm $1.iv cells
~/Progs/collapse/bin/pgm2cca $1.pgm $1.cc
distc $1.pgm 6 dist.pgm
~/Progs/collapse/bin/distance_dilat dist.pgm ddist.pgm
mkdir pardircollapse
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_nofilter_facet.cc facet 0 NULL NULL
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_nofilter_isthmus.cc isthmus 0 NULL NULL
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_dist_facet.cc facet 0 dist.pgm NULL
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_dist_isthmus.cc isthmus 0 dist.pgm NULL
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_ddist_facet.cc facet 0 ddist.pgm NULL
~/Progs/collapse/bin/cca_pardircollapse $1.cc pardircollapse/pardir_ddist_isthmus.cc isthmus 0 ddist.pgm NULL
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_nofilter_facet.cc pardircollapse/pardir_nofilter_facet.iv cells
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_nofilter_isthmus.cc pardircollapse/pardir_nofilter_isthmus.iv cells
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_dist_facet.cc pardircollapse/pardir_dist_facet.iv cells
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_dist_isthmus.cc pardircollapse/pardir_dist_isthmus.iv cells
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_ddist_facet.cc pardircollapse/pardir_ddist_facet.iv cells
~/Progs/collapse/bin/cca2iv pardircollapse/pardir_ddist_isthmus.cc pardircollapse/pardir_ddist_isthmus.iv cells
mkdir lambda
lambdamedialaxis $1.pgm lambda/lambda.pgm

