#ifndef _LIB_INVENTOR_D_H_
#define _LIB_INVENTOR_D_H_

#define SIZE_NAMES 100

//The scene contains various information about the scene, such as the materials array.
//This array of iv_material will be used to keep a trace of all materials of a scene...
typedef struct iv_scene
{
	int32_t num_tabulations;
	FILE *output;
	int32_t open_face;
	int32_t open_edge;
} iv_scene;


#endif
