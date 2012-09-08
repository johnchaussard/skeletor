#ifndef _LIB_INVENTOR_H_
#define _LIB_INVENTOR_H_

#include <libinventor_d.h>
#include <libgeneral.h>
#include <libcomplex.h>


#define FILLED 1
#define INVISIBLE 0


//*******************************************************

iv_scene* inventor_new_scene(char *output_file);
void inventor_delete_scene(iv_scene* scene);
void inventor_add_material_to_scene(iv_scene* scene, char *name, float ambiantR, float ambiantV, float ambiantB, float diffuseR, float diffuseV, float diffuseB, float emissiveR, float emissiveV, float emissiveB, float specularR, float specularV, float specularB, float shininess, float transparency);
void inventor_set_drawstyle(iv_scene *scene, char visibility, uint32_t linewidth, uint32_t pointsize);
void inventor_new_object(iv_scene* scene);
void inventor_close_object(iv_scene* scene);
void inventor_positioncursor(iv_scene* scene);
void inventor_call_defined(iv_scene* scene, char *name);


void inventor_declare_points(iv_scene* scene, char *name, uint32_t* tab_pt, uint32_t num_pt, uint32_t rs, uint32_t ps, char amira_patch);
void inventor_use_previously_defined_points(iv_scene* scene);


void inventor_init_edgeset(iv_scene* scene);
void inventor_close_edgeset(iv_scene* scene);
int32_t inventor_draw_curve(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_edge, uint32_t num_pt_edge);
int32_t inventor_draw_cubicalcomplex_edges(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_edges, uint32_t num_edges, char type_edge, uint32_t rs, uint32_t ps);


void inventor_init_faceset(iv_scene* scene);
void inventor_close_faceset(iv_scene* scene);
int32_t inventor_draw_surface(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_face, uint32_t num_pt_face);
int32_t inventor_draw_cubicalcomplex_faces(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_faces, uint32_t num_faces, char type_face, uint32_t rs, uint32_t ps, int32_t shift);



#endif
