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
#include <libvskel.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<cca_skeleton_image> <vskel_output_file> <pgm_distance> <bounding box : sx sy sz tx ty tz> <grid_filename>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *dist;
	double scale;
	iv_scene* scene;
	list *decomposition, *simplification;
	complexe *temp, *point, *temp2;
	uint32_t i, rs, ps, dim, x, y, z, *tab_weight;
	double sx, sy, sz, tx, ty, tz;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=11)
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



	//We read input image
	dist=readimage(argv[3]);
	if (dist==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
		return(1);
	}
	else if(datatype(dist)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: only 4 bytes PGM image supported\n");
		return(1);
	}


	rs=rowsize(image);
	ps=colsize(image)*rs;


	sx=atof(argv[4]);
	sy=atof(argv[5]);
	sz=atof(argv[6]);
	tx=atof(argv[7]);
	ty=atof(argv[8]);
	tz=atof(argv[9]);


	if(sx>tx || sy>ty || sz>tz)
	{
		fprintf(stderr, "Bounding box is invalid.\n");
		return(1);
	}


	tx = tx-sx;
	ty = ty-sy;
	tz = tz-sz;


	if(tx >= ty && tx >= tz)
	{
		scale = tx/(depth(image)-1);
	}
	else if(ty >= tx && ty >= tz)
	{
		scale = ty/(rowsize(image)-1);
	}
	else
	{
		scale = tz/(colsize(image)-1);
	}



	scene=vskel_new_scene(argv[2], sx, sy, sz, scale);
	if(scene==NULL)
	{
		fprintf(stderr, "Could not create output file %s.\n", argv[2]);
		freeimage(image);
		return(1);
	}










	//********************************************************
	//Make skeleton decomposition
	//********************************************************

	//The image will be changed by the decomposition function, so we first save the points into a complex
	point=complexe_new_complexe();
	if(point==NULL)
	{
		fprintf(stderr, "Could not create new complex structure.\n");
		freeimage(image);
		vskel_delete_scene(scene);
		return(1);
	}

	if(cca_scan_all_vertex_into_complex(image, point) != 0)
	{
		fprintf(stderr, "Error in cca_scan_all_vertex_into_complex() function.\n");
		freeimage(image);
		vskel_delete_scene(scene);
		complexe_free_complexe(point);
		return(1);
	}

	//We can perform decomposition
	decomposition=cca_decompose_skeleton_into_elements(image);
	if(decomposition==NULL)
	{
		fprintf(stderr, "Error in the decomposition function.\n");
		freeimage(image);
		vskel_delete_scene(scene);
		complexe_free_complexe(point);
		return(1);
	}

	simplification=list_newlist(MODE_FIFO, list_getsize(decomposition));
	if(!simplification)
	{
		fprintf(stderr, "Error when creating new list.\n");
		return(1);
	}



	//Get rid of intersection elements
	temp=(complexe*)list_pop_pointer(decomposition);
	complexe_free_complexe(temp);






	//*********************************************************
	//Prepare the scene
	//*********************************************************

	//And send the points to the output
	vskel_declare_points(scene, point->tab_pt_obj, point->num_pt_obj, rs, ps, sx, sy, sz, scale);



	//And the weights
	tab_weight = (uint32_t*)malloc(point->num_pt_obj * sizeof(uint32_t));
	if(tab_weight==NULL)
	{
		fprintf(stderr, "Memory allocation error.\n");
		return(1);
	}

	for(i=0; i<point->num_pt_obj; i++)
	{
		x=getxfrompixnum((point->tab_pt_obj)[i], rs, ps);
		y=getyfrompixnum((point->tab_pt_obj)[i], rs, ps);
		z=getzfrompixnum((point->tab_pt_obj)[i], rs, ps);
		tab_weight[i] = cca_getfacedist(image, dist, x, y, z, CC_PT, rs, ps);
	}

	vskel_declare_weights(scene, tab_weight, point->num_pt_obj);



	//We don't need the image anymore
	freeimage(image);





	//*******************************************************
	//Send the elements to output file
	//*******************************************************

	i=0;
	vskel_init_edgeset(scene);

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

			temp2 = complexe_perform_curve_subsampling(temp, rs, ps, 1.0);
			if (!temp2)
			{
				fprintf(stderr, "Subsampling failed.\n");
				return(-1);
			}

			list_push_pointer(simplification, temp2);
		}
		else
		{
			fprintf(stdout, "Error, complex is two or more dimensional. This is not yet supported.\n");
			return(1);
		}

		complexe_to_vskel(scene, temp, point->num_pt_obj, point->tab_pt_obj, rs, ps);

		fflush(scene->output);

		complexe_free_complexe(temp);

		i++;
	}

	vskel_close_edgeset(scene);



	//***************************************************
	//Send elements from simplification list to file
	//***************************************************

	i=0;
	vskel_init_simplified_edgeset(scene);

	while(!list_isempty(simplification))
	{
		temp=(complexe*)list_pop_pointer(simplification);

		complexe_keep_highest_dimension_faces(temp);

		complexe_to_vskel(scene, temp, point->num_pt_obj, point->tab_pt_obj, rs, ps);

		fflush(scene->output);

		complexe_free_complexe(temp);

		i++;
	}

	vskel_close_edgeset(scene);


	//*********************************************
	//Finished
	//*********************************************

	fprintf(scene->output, "grid %s %f %f %f %f\n", argv[10], -sx, -sy, -sz, 1.0/scale);

	list_delete(decomposition, NO_FREE_DATA);
	list_delete(simplification, NO_FREE_DATA);
	complexe_free_complexe(point);
	vskel_delete_scene(scene);

	return(0);
}


