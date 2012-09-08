
/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libinventor.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<cca_image> <cca_overcollapse> <intersect_edges_cca_image> <inventor_output_file>"




int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *edges, *overcoll;
	char mode_amira;
	double r, v, b;
	iv_scene* scene;
	list *ss_result;
	complexe *temp, *intersect_edges, *point;
	uint32_t i, rs, ps, keep, num_pt, *tab_pt, kept;
	char name[BASE_ALLOC];
	long ltime;

	ltime=time(NULL);
	srand(ltime);


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=5)
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

	overcoll=readimage(argv[2]);
	if (overcoll==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(-1);
	}
	else if(datatype(overcoll)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}

	edges=readimage(argv[3]);
	if (edges==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
		return(-1);
	}
	else if(datatype(edges)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}


	rs=rowsize(image);
	ps=colsize(image)*rs;

	sprintf(name, "%s.iv", argv[4]);
	scene=inventor_new_scene(name);

	//Initialize the scene...
	if(scene==NULL)
	{
		fprintf(stderr, "Error when creating new scene.\n");
		return(-1);
	}

	mode_amira=0;


	//********************************************************
	//Make surface segmentation
	//********************************************************

	//Get the intersection edges
	intersect_edges=cca_to_complexe(edges);
	//freeimage(edges);
	if(intersect_edges==NULL)
	{
		fprintf(stderr, "Error in function cca_to_complexe()\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return (-1);
	}


	//Make separation of the complex into surface components
	ss_result=cca_simple_surface_segmentation_with_intersection_edges_and_overcollapse(image, intersect_edges, overcoll);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}
	//We don't need the image anymore
	freeimage(image);

	fprintf(stdout, "Found %d surfaces\n", list_getsize(ss_result) -2);


	//The first item is the set of all vertices...
	temp=(complexe*)list_pop_pointer(ss_result);

	for(i=0; i<ss_result->size; i++)
	{
		//Choose a random color for surfaces
		r=((double)rand())/((double)RAND_MAX);
		v=((double)rand())/((double)RAND_MAX);
		b=((double)rand())/((double)RAND_MAX);
		sprintf(name, "Surf_%d", i);
		inventor_add_material_to_scene(scene, name, r, v, b, r/2.0, v/2.0, b/2.0, r/3.0, v/3.0, b/3.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	//All the surfaces will go in the same file.
	//The array of points therefore interest us, we keep it and write it in the inventor file
	point=temp;
	tab_pt=point->tab_pt_obj;
	num_pt=point->num_pt_obj;
	inventor_new_object(scene);
	//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
	inventor_declare_points(scene, (char*)"Points", tab_pt, num_pt, rs, ps, mode_amira);




	//The second item is the set of all intersect edges... We don't care.
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);






	//*******************************************************
	//Send the surfaces to output file
	//*******************************************************

	i=0; //Will be used for the filename
	kept=0;
	while(!list_isempty(ss_result))
	{
		//Get the object to write
		temp=(complexe*)list_pop_pointer(ss_result);

		keep=1;
		if(keep==1)
		{

/*			if(temp->num_fxy>0) {k=temp->tab_fxy[0]; f=CC_FXY;}
			else if(temp->num_fxz>0) {k=temp->tab_fxz[0]; f=CC_FXZ;}
			else if(temp->num_fyz>0) {k=temp->tab_fyz[0]; f=CC_FYZ;}
			else assert(0);

			l=cca_list_container(cca_image, k, getxfrompixnum(k, rs, ps), getyfrompixnum(k, rs, ps), getzfrompixnum(k, rs, ps), f, rs, ps);
			assert(l->cpt==2);
			while(!list_isempty(l))
			{
				g=(face_desc*)list_pop(l);
				surf[kept][l->cpt -1]=LONGDATA(labels)[g->pixnumber];
				free(g);
			}
			assert(surf[kept][0] != surf[kept][1]);
			if(surf[kept][0] > surf[kept][1])
			{
				k=surf[kept][1];
				surf[kept][1]=surf[kept][0];
				surf[kept][0]=k;
			}
*/
			kept++;



			inventor_new_object(scene);



			sprintf(name, "Surf_%d", i);

			//inventor_call_defined(scene, "Points");
			complexe_to_inventor(scene, temp, num_pt, tab_pt, rs, ps, mode_amira, NULL, NULL, name, NULL);

			inventor_close_object(scene);

			fflush(scene->output);





		}
		i++;
		complexe_free_complexe(temp);
	}


	inventor_close_object(scene);



	fprintf(stdout, "Kept %d surfaces\n", kept);

	/*for(i=0; i<kept-1; i++)
		for(k=i+1; k<kept; k++)
		{
			if(
*/


	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	list_delete(ss_result, NO_FREE_DATA);


	return(0);
}
