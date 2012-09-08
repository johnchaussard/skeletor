#ifndef _LIB_EPS_H_
#define _LIB_EPS_H_

#include <libeps_d.h>

eps_scene* eps_new_scene(FILE *output_file, int zoom, int max_x, int max_y);
void eps_delete_scene(eps_scene* scene);
void eps_init_scene(eps_scene* scene);
void eps_close_scene(eps_scene* scene);
void eps_define_square(eps_scene* scene, float r, float g, float b, char *name);
void eps_define_hline(eps_scene* scene, float r, float g, float b, char *name);
void eps_define_vline(eps_scene* scene, float r, float g, float b, char *name);
void eps_define_point(eps_scene* scene, float r, float g, float b, char *name);
void eps_draw_element(eps_scene* scene, char* name, int posx, int posy);




#endif
