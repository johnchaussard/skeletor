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
#include <libinventor.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<cca_image> <intersect_edges_cca_image> <inventor_output_file_no_extension> [-amira]"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *edges;
	FILE *output;
	char mode_amira;
	iv_scene* scene;
	list *ss_result, *ss_result2;
	complexe *temp, *intersect_edges;
	uint32_t i, rs, ps, lim, erode;
	char name[BASE_ALLOC];



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

	edges=readimage(argv[2]);
	if (edges==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(-1);
	}
	else if(datatype(edges)!=VFF_TYP_1_BYTE)
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


	rs=rowsize(image);
	ps=colsize(image)*rs;


	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);

	scene=inventor_new_scene(10, NULL);

	//Initialize the scene...
	if(scene==NULL)
	{
		fprintf(stderr, "Error when creating new scene.\n");
		return(-1);
	}

	//We add to our object the main materials :
	inventor_add_material_to_scene(scene, POS_PT, 0.0, 0.0, 0.0, 0.1, 0.4, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_AX, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_AY, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_AZ, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_FXY, 0.0, 0.0, 0.0, 0.65, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_FXZ, 0.0, 0.0, 0.0, 0.50, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_FYZ, 0.0, 0.0, 0.0, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_VXY, 0.0, 0.0, 0.0, 0.65, 0.65, 0.65, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_VXZ, 0.0, 0.0, 0.0, 0.50, 0.50, 0.50, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, POS_VYZ, 0.0, 0.0, 0.0, 0.80, 0.80, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


	//********************************************************
	//Make surface segmentation
	//********************************************************

	//Get the intersection edges
	intersect_edges=cca_to_complexe(edges);
	if(intersect_edges==NULL)
	{
		fprintf(stderr, "Error in function cca_to_complexe()\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return -1;
	}
	freeimage(edges);

	edges=complexe_to_cca(intersect_edges, rs, colsize(image), depth(image));
	writeimage(edges, "debug.cc");

	ss_result=cca_simple_surface_segmentation(image);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}

	fprintf(stdout, "Found %d surfaces\n", ss_result->cpt -2);

	temp=list_pop(ss_result); //The first item is the set of all vertices... We don't care.
	complexe_free_complexe(temp);
	temp=list_pop(ss_result); //The second item is the set of all intersect edges... We don't care.
	complexe_free_complexe(temp);


	freeimage(image);

	i=0; //Will be used for the filename
	while(!list_isempty(ss_result))
	{
		//Get the object to write
		temp=list_pop(ss_result);

		//Create an output file for the surface
		sprintf(name, "%s.surf%d.%d.iv", argv[3], temp->num_fxy+temp->num_fyz+temp->num_fxz, i);
		output = fopen(name, "wb");
		if(output==NULL)
		{
			fprintf(stderr, "Error: could not create output file %s (directory exists?).\n", name);
			list_delete(ss_result, FREE_DATA); //we should do a system with a function pointer
			inventor_delete_scene(scene);
			return(-1);
		}

		//Link the output with the scene
		scene->output=output;

		//And write the complex object in the scene
		inventor_init_file(scene);
		complexe_compute_vertex_array(temp, rs, ps);
		inventor_new_object(scene);
		inventor_set_drawstyle(scene, FILLED, 1, 1);
		complexe_to_inventor(scene, temp, rs, ps, DRAW_NONE, mode_amira);
		inventor_close_object(scene);

		fflush(scene->output);
		fclose(scene->output);
		i++;

		complexe_free_complexe(temp);
	}


	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	list_delete(ss_result, NO_FREE_DATA);


	return(0);
}

