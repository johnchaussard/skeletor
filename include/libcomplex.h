#ifndef _LIB_COMPLEX_H_
#define _LIB_COMPLEX_H_

#include <libcomplex_d.h>
#include <liblist.h>
#include <libinventor.h>
#include <libeps_d.h>

//We define different configuration codes in order to know the voxel's surroundings (it has which faces, which edges, etc...)
//Each voxel codes for one vertex, three edges, three faces and one volume
//F1 is the plane with z fixed, F2 is the plane with y fixed, F3 is the plane with x fixed
//A1 is the edge in z, A2 is the edge in y, A3 is the edge in x
#define EMPTY 0
#define CC_PT 1
#define CC_AZ 2
#define CC_AY 4
#define CC_AX 8
#define CC_FXY 16
#define CC_FXZ 32
#define CC_FYZ 64
#define CC_VOL 128


//For general complexe structure
#define GC_PT  1
#define GC_EDGE 3
#define GC_FACE 5


//For general double complexe structure
#define D_PT  7
#define D_EDGE 9
#define D_FACE 11


//Codes of position, for each basic elements...
#define POS_PT 0
#define POS_AZ 1
#define POS_AY 2
#define POS_AX 3
#define POS_FXY 4
#define POS_FXZ 5
#define POS_FYZ 6
#define POS_VXY 7
#define POS_VXZ 8
#define POS_VYZ 9


//Codes of material position in scene file (iv or eps)
#define POS_MAT_PT 0
#define POS_MAT_EDGE 1
#define POS_MAT_SQUARE 2


//Return codes of cca_is_face_free
//Codes for the direction to collapse a free face... For each free face, we define the different directions in which it could be collapsed.
//DO not touch these codes, or you will also need to modify the free face function which work on bit operations
#define CC_NOT_FREE_NONEIGHBOUR 0 //Code used for faces, edge and vertices
#define CC_NOT_FREE_PROTECTED 1 //Code used in case of inhibit image
#define CC_NOT_FREE_TWONEIGHBOURS 64 //Code used for faces
#define CC_FACE_COLL_DOWN 128 //Code used for faces
#define CC_FACE_COLL_UP 192 //Code used for faces
#define CC_NO_SUCH_FACE 255 //Code used for faces, edges and vertices
#define CC_EDGE_COLL_DIR1 8 //Code used for edges
#define CC_EDGE_COLL_DIR2 9 //Code used for edges
#define CC_EDGE_COLL_DIR3 10 //Code used for edges
#define CC_EDGE_COLL_DIR4 12 //Code used for edges
#define CC_VERT_COLL_XUP 32 //Code used for vertices
#define CC_VERT_COLL_XDOWN 33 //Code used for vertices
#define CC_VERT_COLL_YUP 34 //Code used for vertices
#define CC_VERT_COLL_YDOWN 36 //Code used for vertices
#define CC_VERT_COLL_ZUP 40 //Code used for vertices
#define CC_VERT_COLL_ZDOWN 48 //Code used for vertices

//Return codes of cca_is_face_on_border
#define X_BORDER_MIN 1
#define X_BORDER_MAX 2
#define Y_BORDER_MIN 4
#define Y_BORDER_MAX 8
#define Z_BORDER_MIN 16
#define Z_BORDER_MAX 32

//Used in the complexe functions in order to make allocation
#define BASE_ALLOC 1000

//Used in cca_to_inventor function to know what to do with the vertex of the image : draw none or draw the ones of the image ?
//Also used in surface_segmentation function to know if we should draw only object's cells or all complex...
#define DRAW_NONE 0
#define DRAW_PT 1
#define DRAW_ALL 2
#define DRAW_CELLS 3

#define TRUE 1
#define FALSE 0

#define ISFREE(f,t) (t!=CC_NO_SUCH_FACE && ( (f>=CC_FXY && t>=CC_FACE_COLL_DOWN) || (f<CC_FXY && f!=CC_PT && t>=CC_EDGE_COLL_DIR1) || (f==CC_PT && t>=CC_VERT_COLL_XUP) ))


#define CCA_COMPLEXE 0
#define GENERAL_COMPLEXE 1

#define KEEP 0
#define REJECT 1



//Functions are split in different objects files

//libcomplex_collapse.c
uint32_t cca_is_opposite_face_free(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, unsigned char face_contained, unsigned char direction_free, uint32_t rs, uint32_t ps);
uint32_t cca_is_free_face_total_vise_free(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, unsigned char facetype, uint32_t rs, uint32_t ps);
void cca_collapse_face(struct xvimage *cca_image, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction);
int32_t cca_is_collapse_inside_face(unsigned char face_container_type, uint32_t face_container_pixnum, unsigned char face_collapse_type, uint32_t face_collapse_pixnum, unsigned char direction, uint32_t rs, uint32_t ps);
int32_t cca_collapse_face_inside_container(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, uint32_t container_pixnumber, unsigned char container_facetype, list **free_faces, list **empty_lists);
int32_t cca_collapse_face_inside_container_with_two_images(struct xvimage *original_image, struct xvimage *collapsed_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, uint32_t container_pixnumber, unsigned char container_facetype, list **free_faces, list **empty_lists);
int32_t cca_collapse_face_and_subfaces(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, list **free_faces);
int32_t cca_collapse_face_and_subfaces_with_two_images(struct xvimage *original_image, struct xvimage *collapsed_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, list **free_faces);
int32_t cca_collapse(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop);
int32_t cca_collapse_method_facesandsubfaces(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop);
int32_t cca_collapse_method_facesandsubfaces2(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop);
int32_t cca_collapse_method_facesandsubfaces_avoid_intersections(struct xvimage *cca_image, struct xvimage *inhibit);
int32_t cca_collapse_minimize_intersections(struct xvimage *cca_image, struct xvimage* inhibit);
int32_t cca_collapse_cubes_avoid_create_surfaces(struct xvimage *cca_image, struct xvimage* inhibit);
int32_t cca_surfacic_collapse(struct xvimage *cca_image);
int32_t cca_add_free_face_to_list(list **free_faces, unsigned char facetype, unsigned char dir, uint32_t pixnum);
unsigned char cca_get_face_type_from_list_index(uint32_t index);
unsigned char cca_get_collapse_direction_from_list_index(uint32_t index);
list **cca_collapse_directional_surfacic(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, list **border_faces);
list **cca_collapse_directional_surfacic_for_pgm_purpose(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, struct xvimage *cube_survive, uint32_t *cpt_cube_survive);
list **cca_collapse_directional_surfacic_enhanced(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, struct xvimage **temp_border_complex);
void cca_check_free_faces_after_collapse(struct xvimage *cca_image, struct xvimage *inhibit, unsigned char face_collapse, unsigned char direction, list **result, uint32_t pixnumber, uint32_t rs, uint32_t ps);
void cca_check_free_faces_after_collapse_dirstrat(struct xvimage *cca_image, struct xvimage *inhibit, unsigned char face_collapse, unsigned char direction, list **insiders, list **oustiders, uint32_t pixnumber, uint32_t rs, uint32_t ps);
int32_t cca_vsa_collapse_keep_faces(struct xvimage *cca_image);
int32_t cca_perso(struct xvimage *cca_image, complexe *rvsa, struct xvimage *rvsa_image);
void cca_check_free_faces_in_cube(struct xvimage *cca_image, struct xvimage *inhibit, list **result, uint32_t pixnumber, uint32_t rs, uint32_t ps);

//libcomplex_convert.c
int32_t pgm_to_cca(struct xvimage *image);
struct xvimage *cca_to_khalimsky3d(struct xvimage *cca_image);
struct xvimage *khalimsky3d_to_cca(struct xvimage *khal_image);
int32_t cca_to_inventor(struct xvimage *cca_image, iv_scene *scene, char point_behaviour, char amira_patch, char mode_vrml);
int32_t cca_to_eps(struct xvimage *cca_image, eps_scene *scene);
uint32_t complexe_to_inventor(iv_scene* scene, complexe* object, uint32_t num_pt_scene, uint32_t* tab_pt_scene, uint32_t rs, uint32_t ps, char amira_patch, char *name_matpt, char *name_matedge, char *name_matface, char *name_matvol);
void complexe_add_to_cca(struct xvimage *input, complexe* object);
void complexe_remove_from_cca(struct xvimage *input, complexe* object);
struct xvimage* complexe_to_cca(complexe* object, uint32_t rs, uint32_t cs, uint32_t d);
complexe* cca_to_complexe(struct xvimage* cca_image);
complexe *cc_to_gc_only_simple_edges(complexe *only_edges, uint32_t max_euc_square_dist, uint32_t rs, uint32_t ps, uint32_t N);
complexe *cc_to_gc_one_surface(list* gcomplexe_contour, uint32_t rs, uint32_t ps, uint32_t N);
int32_t complexe_to_cplfile(complexe *cpl, FILE *f);
uint32_t complexe_to_vskel(iv_scene* scene, complexe* object, uint32_t num_pt_scene, uint32_t* tab_pt_scene, uint32_t rs, uint32_t ps);

//libcomplex_general.c
list *cca_list_subfaces_dimension_direct_under(uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t ps);
list *cca_list_starface_dimension_direct_over(uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t cs, uint32_t d);
void cca_add_to_list_subfaces_dimension_direct_under(list *l, face_desc *fd, uint32_t rs, uint32_t ps);
void cca_add_to_list_starface_dimension_direct_over(struct xvimage* image, list *l, face_desc *fd, uint32_t rs, uint32_t cs, uint32_t d);
complexe* complexe_downsample_point_resolution(complexe *c, uint32_t rs, uint32_t ps, double ratio);
uint32_t complexe_intersects_image(complexe *c, struct xvimage *i);
int32_t cca_is_face_isthmus(struct xvimage* cca_image, uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t ps);
face_desc cca_get_container_of_free_face(unsigned char facetype, uint32_t pixnumber, uint32_t direction, uint32_t rs, uint32_t ps);
face_desc cca_get_common_container(struct xvimage* cca_image, unsigned char facetype1, uint32_t pixnumber1, unsigned char facetype2, uint32_t pixnumber2);
uint32_t cca_get_face_dimension(unsigned char facetype);
uint32_t cca_get_number_container(unsigned char facetype);
face_desc cca_get_one_container(uint32_t pixnumber, unsigned char facetype, uint32_t container_number, uint32_t x, uint32_t y, uint32_t z, struct xvimage* cca_image);
uint32_t cca_getfacedist(struct xvimage *cca_image, struct xvimage *distmap, uint32_t i, uint32_t j, uint32_t k, unsigned char face_type, uint32_t rs, uint32_t ps);
void cca_add_complexface(struct xvimage *image, unsigned char face_type, uint32_t pixnumber, uint32_t rs, uint32_t ps);
void cca_remove_complexface(struct xvimage *image, unsigned char face_type, uint32_t pixnumber, uint32_t rs, uint32_t ps);
uint32_t cca_is_face_onborder(struct xvimage *cca_image, int32_t face_type, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z);
uint32_t cca_is_face_onborder_v2(int32_t face_type, uint32_t pixnumber, uint32_t rs, uint32_t cs, uint32_t d);
void cca_keep_only_cells(struct xvimage* image);
unsigned char cca_is_face_free(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type);
void cca_makecomplex(struct xvimage *cca_image);
uint32_t cca_cardinal_containers(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps);
uint32_t cca_edge_cube_cardinal_containers(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps);
int32_t cca_scan_all_vertex_into_complex(struct xvimage *cca_image, complexe *result);
int32_t cca_get_all_edges_with_given_cardinalcontainer(struct xvimage *cca_image, complexe* result, uint32_t cardinal_container);
list* cca_list_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps);
uint32_t cca_count_face_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps);
uint32_t cca_count_cube_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps);
int32_t cca_get_border_faces_parallel_to_face(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps, uint32_t *r1, uint32_t *r2);
uint32_t cca_get_face_normaldepth(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps);
uint32_t cca_get_face_rvsa_depth(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps);
complexe* cca_compute_border_faces(struct xvimage* cca_image);
complexe* cca_compute_vise_axis(struct xvimage* cca_image);
complexe* cca_compute_vise_axis_internal(struct xvimage* cca_image, complexe* border_faces);
complexe* cca_compute_vise_surfacic_axis(struct xvimage* cca_image, uint32_t alpha);
complexe* cca_compute_vise_surfacic_axis_internal(struct xvimage* cca_image, uint32_t alpha, complexe *border_faces);
complexe *cca_geodilat_element_to_complex(face_desc* data, struct xvimage *image, uint32_t rs, uint32_t ps, uint32_t N);
complexe *cca_geodilat_simplecurve_to_complex(face_desc* data, struct xvimage *image, uint32_t rs, uint32_t ps, uint32_t N);
int32_t cca_geodilat(struct xvimage *image, struct xvimage *container, struct xvimage *avoid);
list **cca_link_surfaces_to_border_edges(list *surfaces, list *border_edges, uint32_t rs, uint32_t ps, uint32_t N);
list **cca_link_border_edges_to_surfaces(list *surfaces, list *border_edges, uint32_t rs, uint32_t ps, uint32_t N);
uint32_t does_Line_Intersect_With_Face(uint32_t pt_face, unsigned char type_face, uint32_t rs, uint32_t ps, double* pt_space, double *direction, double *result);
void cca_move_subfaces_to_list(struct xvimage *image, list *l, face_desc *fd, uint32_t rs, uint32_t ps);
void cca_move_starfaces_to_list(struct xvimage *image, list *l, face_desc *fd, uint32_t rs, uint32_t ps);
uint32_t cca_get_cardinal_subface(unsigned char facetype);
void cca_get_subface(unsigned char facetype, uint32_t pixnumber, uint32_t subface_index, face_desc *result, uint32_t rs, uint32_t ps);
void cca_get_starface(unsigned char facetype, uint32_t pixnumber, uint32_t starface_index, face_desc *result, uint32_t rs, uint32_t ps, uint32_t border_code);
uint32_t cca_get_cardinal_subface(unsigned char facetype);
uint32_t cca_get_cardinal_starface(unsigned char facetype);
uint32_t cca_get_cardinal_specific_starface_in_image(struct xvimage* image, uint32_t facetype, uint32_t pixnumber, uint32_t dimension_starface, uint32_t rs, uint32_t ps, uint32_t border_code);
uint32_t cca_face_is_an_intersection_face(struct xvimage *image, unsigned char facetype, uint32_t pixnumber, uint32_t rs, uint32_t ps, uint32_t border_code);
uint32_t complexe_get_num_faces(complexe *cpl);
uint32_t complexe_count_faces_inside_cca(complexe *cpl, struct xvimage* image);
void complexe_keep_highest_dimension_faces(complexe *cpl);
uint32_t complexe_is_partly_inside_cca(complexe *cpl, struct xvimage *image);
uint32_t complexe_get_dimension(complexe* cpl);
uint32_t complexe_optimize_simple_curve(complexe *cpl, uint32_t rs, uint32_t ps);
complexe* complexe_perform_curve_subsampling(complexe *cpl, uint32_t rs, uint32_t ps, double tolerance);

//libcomplex_surfaces.c
list *cca_simplify_surfaces_wth_border(list *lst_surfaces, list *lst_edges, uint32_t simplification_factor, uint32_t rs, uint32_t ps, uint32_t N);
int32_t cca_get_simple_surface(struct xvimage *cca_image, complexe* result, uint32_t one_face, unsigned char typ_face);
list* cca_simple_surface_segmentation_with_intersection_edges(struct xvimage* image, complexe* intersection_edges);
list* cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image(struct xvimage* image, complexe* intersection_edges, struct xvimage *keep_reject_image, unsigned char strategy);
list* cca_simple_surface_segmentation(struct xvimage* image);
list* cca_simple_curve_decomposition(struct xvimage* edges, complexe *non_regular_vertex);
list* cca_simple_surface_segmentation_with_intersection_edges_and_overcollapse(struct xvimage* image, complexe* intersection_edges, struct xvimage* overcollapse);
int32_t cca_get_simple_local_surface(struct xvimage *cca_image, complexe* result, uint32_t px, unsigned char face);
list* cca_simple_curve_decomposition_non_optimized(struct xvimage* edges, complexe *non_regular_vertex);
list* cca_decompose_into_connected_elements(struct xvimage* image);
void complexe_geodilat_inside_cca(list *decomposition, struct xvimage *cca_image);
list *cca_decompose_skeleton_into_elements(struct xvimage* image);
void complexe_geodilat_inside_cca(list *decomposition, struct xvimage *cca_image);

//libcomplex_data.c
uint32_t complexe_get_numvertex_of_element(complexe *cpl, uint32_t num_element, unsigned char typ_face);
uint32_t complexe_get_vertex_of_element(complexe *cpl, uint32_t num_element, uint32_t num_vertex, unsigned char typ_face);
uint32_t complexe_get_vertex_position_in_element(complexe *cpl, uint32_t num_element, uint32_t num_vertex, unsigned char typ_face);
int32_t complexe_close_element(complexe *cpl, unsigned char typ_element);
int32_t complexe_add_element(complexe *cpl, uint32_t data, unsigned char typ_face);
int32_t complexe_add_array_element(complexe *cpl, uint32_t* data, uint32_t num_element_to_add, unsigned char typ_face);
complexe* complexe_new_complexe();
void complexe_free_complexe(complexe* data);
int32_t qsort_function_on_uint32(const void* inta, const void* intb);
int32_t qsort_function_on_float(const void* inta, const void* intb);
void complexe_compute_vertex_array(complexe *data, uint32_t rs, uint32_t ps);
void complexe_remove_faces(complexe *data, unsigned char typ_face);

//libcomplex_dist.c
struct xvimage *compute_closest_cca_vertex_to_pgm(struct xvimage *cca_image, struct xvimage *mask, struct xvimage *border, struct xvimage *distc6);


#endif
