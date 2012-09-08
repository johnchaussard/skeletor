#ifndef _LIB_EPS_D_H_
#define _LIB_EPS_D_H_

#define SIZE_NAMES 100

//The scene contains various information about the scene, such as the materials array.
//This array of iv_material will be used to keep a trace of all materials of a scene...
typedef struct eps_scene
{
	int do_not_access_tx; //Should be private
	int do_not_access_ty; //Should be private
	int zoom;
	int width;
	int height;
	FILE *output;
} eps_scene;


#endif
