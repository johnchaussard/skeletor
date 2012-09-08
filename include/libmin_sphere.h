#ifndef _LIB_MIN_SPHERE_H_
#define _LIB_MIN_SPHERE_H_


#define EPSILON 0.000001  //For floating points comparisons...

inline double det_matrice_4(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, double k, double l, double m, double n, double o, double p);
int32_t compute_min_disk_with_border_constraint(double *tab_point, uint32_t num_point, double *tab_point_border, uint32_t num_point_border, double *c_x, double *c_y, double *c_r);
int32_t compute_min_sphere_with_border_constraint(double *tab_point, uint32_t num_point, double *tab_point_border, uint32_t num_point_border, double *c_x, double *c_y, double *c_z, double *c_r);
int32_t build_circle(double *tab_coord, uint32_t num_points, double* c_x, double* c_y, double* c_r);
int32_t build_sphere(double *tab_coord, uint32_t num_points, double* c_x, double* c_y, double* c_z, double* c_r);

#endif
