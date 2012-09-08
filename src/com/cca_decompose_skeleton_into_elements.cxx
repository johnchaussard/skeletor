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

#define USAGE "<cca_skeleton_image> <inventor_output_file> [-amira]"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	char mode_amira, name[20], name_curv[20], name_face[20], name_vert[20];
	double r, v, b;
	iv_scene* scene;
	list *decomposition;
	complexe *temp, *point, *temp2;
	uint32_t i, rs, ps, dim;
	long ltime;

	ltime=time(NULL);
	srand(ltime);



	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		return(1);
	}



	mode_amira=0;
	if(argc==5)
	{
		if(strcmp(argv[4], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			freeimage(image);
			return(1);
		}
		else
		{
			mode_amira=1;
		}
	}


	scene=inventor_new_scene(argv[2]);
	if(scene==NULL)
	{
		fprintf(stderr, "Could not create output file %s.\n", argv[2]);
		freeimage(image);
		return(1);
	}

	rs=rowsize(image);
	ps=colsize(image)*rs;



	//********************************************************
	//Make skeleton decomposition
	//********************************************************

	//The image will be changed by the decomposition function, so we first save the points into a complex
	point=complexe_new_complexe();
	if(point==NULL)
	{
		fprintf(stderr, "Could not create new complex structure.\n");
		freeimage(image);
		inventor_delete_scene(scene);
		return(1);
	}

	if(cca_scan_all_vertex_into_complex(image, point) != 0)
	{
		fprintf(stderr, "Error in cca_scan_all_vertex_into_complex() function.\n");
		freeimage(image);
		inventor_delete_scene(scene);
		complexe_free_complexe(point);
		return(1);
	}

	//We can perform decomposition
	decomposition=cca_decompose_skeleton_into_elements(image);
	if(decomposition==NULL)
	{
		fprintf(stderr, "Error in the decomposition function.\n");
		freeimage(image);
		inventor_delete_scene(scene);
		complexe_free_complexe(point);
		return(1);
	}

	//We don't need the image anymore
	freeimage(image);

	//Get rid of intersection elements
	temp=(complexe*)list_pop_pointer(decomposition);
	complexe_free_complexe(temp);






	//*********************************************************
	//Prepare the scene
	//*********************************************************

	//For each element found, we create a material in the output inventor file

	for(i=0; i<decomposition->size; i++)
	{
		//Choose a random color for elements
		r=((double)rand())/((double)RAND_MAX);
		v=((double)rand())/((double)RAND_MAX);
		b=((double)rand())/((double)RAND_MAX);
		sprintf(name, "Point_%d", i); //In case we had a single point element
		inventor_add_material_to_scene(scene, name, 0.0, 0.0, 0.0, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		sprintf(name, "Curv_%d", i);
		inventor_add_material_to_scene(scene, name, 0.0, 0.0, 0.0, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		sprintf(name, "Surf_%d", i);
		inventor_add_material_to_scene(scene, name, r, v, b, r/2.0, v/2.0, b/2.0, r/3.0, v/3.0, b/3.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	//And send the points to the output
	inventor_new_object(scene);
	inventor_declare_points(scene, (char*)"Points", point->tab_pt_obj, point->num_pt_obj, rs, ps, mode_amira);




	//*******************************************************
	//Send the elements to output file
	//*******************************************************

	i=0;
	while(!list_isempty(decomposition))
	{
		temp=(complexe*)list_pop_pointer(decomposition);

		complexe_keep_highest_dimension_faces(temp);

		dim=complexe_get_dimension(temp);
		if(dim==1)
		{
			if(complexe_optimize_simple_curve(temp, rs, ps)!=0)
			{
				fprintf(stderr, "Error, optimization of simple curve failed.\n");
				return(1);
			}
			temp->num_pt_obj=0;
			free(temp->tab_pt_obj);
			temp->tab_pt_obj=NULL;

//			temp2 = complexe_perform_curve_subsampling(temp, rs, ps, 1.0);
//			if (!temp2)
//			{
//				fprintf(stderr, "Subsampling failed.\n");
//				return(-1);
//			}
//
//			complexe_free_complexe(temp);
//			temp=temp2;
		}


		inventor_new_object(scene);

		sprintf(name_vert, "Point_%d", i);
		sprintf(name_curv, "Curv_%d", i);
		sprintf(name_face, "Surf_%d", i);

		complexe_to_inventor(scene, temp, point->num_pt_obj, point->tab_pt_obj, rs, ps, mode_amira, name_vert, name_curv, name_face, name_face);

		inventor_close_object(scene);

		fflush(scene->output);

		complexe_free_complexe(temp);

		i++;
	}

	inventor_close_object(scene);


	//*********************************************
	//Finished
	//*********************************************

	list_delete(decomposition, NO_FREE_DATA);
	complexe_free_complexe(point);
	inventor_delete_scene(scene);

	return(0);
}

