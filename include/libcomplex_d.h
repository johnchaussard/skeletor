#ifndef _LIB_COMPLEX_D_H_
#define _LIB_COMPLEX_D_H_

#define CCA_COMPLEXE 0
#define GENERAL_COMPLEXE 1
#define DOUBLE_GENERAL_COMPLEXE 2

#define COORDX 0
#define COORDY 1
#define COORDZ 2


typedef struct complexe
{
	unsigned char TYP_COMPLEXE;
	//Common for cubical and general complex
	uint32_t num_pt_obj;
	uint32_t *tab_pt_obj;
	double *dtab_pt_obj[3];
	//Only for cubical complex
	uint32_t num_ax;
	uint32_t *tab_ax;
	uint32_t num_ay;
	uint32_t *tab_ay;
	uint32_t num_az;
	uint32_t *tab_az;
	uint32_t num_fxy;
	uint32_t *tab_fxy;
	uint32_t num_fxz;
	uint32_t *tab_fxz;
	uint32_t num_fyz;
	uint32_t *tab_fyz;
	uint32_t num_vol;
	uint32_t *tab_vol;
	//Only for general complex
	uint32_t num_edges;
	uint32_t *tab_edges; //Each element points at the end of the current edge
	uint32_t num_edges_pts;
	uint32_t *tab_edges_pts; //One array with all edge's points placed one after the other
	uint32_t num_faces;
	uint32_t *tab_faces; //Each element points at the end of the current edge
	uint32_t num_faces_pts;
	uint32_t *tab_faces_pts; //One array with all edge's points placed one after the other
} complexe;

typedef struct face_desc
{
	uint32_t pixnumber;
	unsigned char face_type;
} face_desc;

typedef struct face_deep{
	uint32_t pixnum;
	uint32_t deepness;
} face_deep;

typedef struct collapsedir{
	unsigned char face;
	unsigned char direction;
} collapsedir;

#endif
