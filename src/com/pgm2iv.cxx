/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libcomplex.h>
#include <libinventor.h>
#include <libvrml.h>


#define USAGE "<pgm_image> <inventor_output_file> <all|cells> <iv|vrml> [-amira]"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	char mode_amira, mode_pt, mode_vrml;
	uint32_t N, rs, ps, i;
	iv_scene *scene;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc<5 || argc>6)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only 1 byte per pixel PGM image supported\n");
		return(-1);
	}

	mode_amira=0;
	if(argc==6)
	{
		if(strcmp(argv[5], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			return(-1);
		}
		else
		{
			mode_amira=1;
		}
	}

	if(	(strcmp(argv[3], "all")!=0 && strcmp(argv[3], "cells")!=0) ||
		(strcmp(argv[4], "vrml")!=0 && strcmp(argv[4], "iv")!=0) )
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		freeimage(image);
		return(-1);
	}




	if(strcmp(argv[4], "vrml")==0)
	{
		mode_vrml=1;
	}
	else
	{
		mode_vrml=0;
	}


	//*******************************************************
	//Conversion of image in cca format...
	//*******************************************************

	//We convert the image to cca format
	if(pgm_to_cca(image)<0)
	{
		fprintf(stderr, "Error: pgm_to_cca() failed.\n");
		freeimage(image);
		return(-1);
	}

	//*******************************************************
	//Removal of unecessary information
	//*******************************************************

	//Then, we scan the image in order to keep only the free faces and volumes
	rs=rowsize(image);
	ps=colsize(image)*rs;
	N=depth(image)*ps;

	for(i=0; i<N; i++)
	{
		if(UCHARDATA(image)[i]!=0)
		{
			UCHARDATA(image)[i] &= (CC_VOL | CC_FXY | CC_FYZ | CC_FXZ); //We remove the edges and vertex...
			if(!ISFREE(CC_FXY, cca_is_face_free(image, NULL, i, rs, ps, CC_FXY))) {UCHARDATA(image)[i] = (UCHARDATA(image)[i] | CC_FXY) - CC_FXY;} //If the XY face is not free, remove it
			if(!ISFREE(CC_FXZ, cca_is_face_free(image, NULL, i, rs, ps, CC_FXZ))) {UCHARDATA(image)[i] = (UCHARDATA(image)[i] | CC_FXZ) - CC_FXZ;} //If the XZ face is not free, remove it
			if(!ISFREE(CC_FYZ, cca_is_face_free(image, NULL, i, rs, ps, CC_FYZ))) {UCHARDATA(image)[i] = (UCHARDATA(image)[i] | CC_FYZ) - CC_FYZ;} //If the YZ face is not free, remove it
		}
	}

	//Then, remove all volumes
	for(i=0; i<N; i++)
	{
		UCHARDATA(image)[i] &= (CC_FXY | CC_FYZ | CC_FXZ); //We remove the volumes
	}

	//And bring back the edges and points
	if(strcmp(argv[3], "all")==0)
	{
		cca_makecomplex(image);
		mode_pt=DRAW_ALL;
	}
	else
	{
		mode_pt=DRAW_NONE;
	}




	//*******************************************************
	//Creation of scene file
	//*******************************************************
	if(mode_vrml ==1)
		scene=vrml_new_scene(argv[2]);
	else
		scene=inventor_new_scene(argv[2]);

	//Initialize the scene...
	if(scene==NULL)
	{
		fprintf(stderr, "Error when creating new scene.\n");
		return(-1);
	}

	//We add to our object the main materials :
	inventor_add_material_to_scene(scene, (char*)"MatPoint", 0.0, 0.0, 0.0, 0.1, 0.4, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, (char*)"MatEdge", 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, (char*)"MatFace", 0.65, 0.0, 0.0, 0.325, 0.0, 0.0, 0.216, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, (char*)"MatVolume", 0.65, 0.65, 0.65, 0.325, 0.325, 0.325, 0.216, 0.216, 0.216, 0.0, 0.0, 0.0, 0.0, 0.0);


	//*******************************************************
	//convert to inventor
	//*******************************************************

	//We convert the cca_image to inventor
	if(cca_to_inventor(image, scene, mode_pt, mode_amira, mode_vrml)<0)
	{
		fprintf(stderr, "Error: cca_to_inventor() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}


	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	freeimage(image);


	return(0);
}
