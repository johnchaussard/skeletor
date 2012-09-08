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
#include <ltangents.h>

//Bibliotheque perso
#include <libinventor.h>
#include <libcomplex.h>

#define USAGE "<cca_image> <inventor_output_file> (all|cells) [-amira]"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	char mode_amira, mode_pt;
	iv_scene* scene;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc<4 || argc>5)
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
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}

	mode_amira=0;
	if(argc==5)
	{
		if(strcmp(argv[4], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			freeimage(image);
			return(-1);
		}
		else
		{
			mode_amira=1;
		}
	}

	if(strcmp(argv[3], "all")!=0 && strcmp(argv[3], "cells")!=0)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		freeimage(image);
		return(-1);
	}

	//*******************************************************
	//Conversion of image to inventor file
	//*******************************************************

	//Keep only cells if necessary...
	if(strcmp(argv[3], "cells")==0)
	{
		cca_keep_only_cells(image);
		mode_pt=DRAW_PT;
	}
	else
	{
		mode_pt=DRAW_ALL;
	}

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



	if(cca_to_inventor(image, scene, mode_pt, mode_amira, 0)<0)
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
