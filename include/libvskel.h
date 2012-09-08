#ifndef _LIB_VSKEL_H_
#define _LIB_VSKEL_H_

#include <libgeneral.h>
#include <libcomplex.h>



//*******************************************************

iv_scene* vskel_new_scene(char *output_file, double tx, double ty, double tz, double scale);


void vskel_delete_scene(iv_scene* scene);
void vskel_positioncursor(iv_scene* scene);
void vskel_declare_points(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t rs, uint32_t ps, double tx, double ty, double tz, double scale);
void vskel_declare_weights(iv_scene* scene, uint32_t* tab_weight, uint32_t num_pt);


void vskel_init_edgeset(iv_scene* scene);
void vskel_init_simplified_edgeset(iv_scene* scene);
void vskel_close_edgeset(iv_scene* scene);
int32_t vskel_draw_curve(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_edge, uint32_t num_pt_edge);
int32_t vskel_draw_cubicalcomplex_edges(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_edges, uint32_t num_edges, char type_edge, uint32_t rs, uint32_t ps);


void vskel_init_faceset(iv_scene* scene);
void vskel_close_faceset(iv_scene* scene);
int32_t vskel_draw_surface(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_face, uint32_t num_pt_face);
int32_t vskel_draw_cubicalcomplex_faces(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_faces, uint32_t num_faces, char type_face, uint32_t rs, uint32_t ps);


#endif

