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

#define USAGE "<input_cca> <output_filtered_cca> <output_iv_rejected_no_extension> [-amira]"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *inhibit, *save;
	FILE *output;
	char mode_amira;
	iv_scene* scene;
	list *ss_result, *ss_result2;
	complexe *vertex, *intersect_edges, *temp;
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

	mode_amira=0;
	if(argc==5)
	{
		if(strcmp(argv[6], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			freeimage(image);
			fclose(output);
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


	if(erode>0)
	{
		fprintf(stdout, "Erosion of image...");
		fflush(stdout);

		temp=complexe_new_complexe();
		if(temp==NULL)
		{
			fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): could not create new complexe.\n");
			return(-1);
		}

		if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 3)<0)
		{
			fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while scanning for surfaces intersections (2).\n");
			return(-1);
		}

		if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 4)<0)
		{
			fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while scanning for surfaces intersections (3).\n");
			return(-1);
		}



		inhibit=allocimage(NULL, rs, colsize(image), depth(image), VFF_TYP_1_BYTE);
		if(inhibit==NULL)
		{
			fprintf(stderr, "Error: copyimage() failed.\n");
			inventor_delete_scene(scene);
			freeimage(image);
			return(-1);
		}


		for(i=0; i<temp->num_ax; i++)
			cca_add_complexface(inhibit, CC_AX, temp->tab_ax[i], rs, ps);

		for(i=0; i<temp->num_ay; i++)
			cca_add_complexface(inhibit, CC_AY, temp->tab_ay[i], rs, ps);

		for(i=0; i<temp->num_az; i++)
			cca_add_complexface(inhibit, CC_AZ, temp->tab_az[i], rs, ps);

		//cca_makecomplex(inhibit);

		cca_collapse(image, inhibit, erode);

		save=copyimage(image);

		freeimage(inhibit);

		complexe_free_complexe(temp);
	}

	/*for(i=0; i<temp->num_ax; i++)
		cca_remove_complexface(image, CC_AX, temp->tab_ax[i], rs, ps);

	for(i=0; i<temp->num_ay; i++)
		cca_remove_complexface(image, CC_AY, temp->tab_ay[i], rs, ps);

	for(i=0; i<temp->num_az; i++)
		cca_remove_complexface(image, CC_AZ, temp->tab_az[i], rs, ps);
*/








	fprintf(stdout, "done\nSurface segmentation... ");
	fflush(stdout);

	ss_result=cca_simple_surface_segmentation(image);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}

	vertex=list_pop(ss_result); //The first item is the set of all vertices... We don't care.

	freeimage(image);
	if(erode==0)
		image=readimage(argv[1]);
	else
		image=save;



	//Remove, by collapse, small surfaces

	if(lim>0)
	{
		fprintf(stdout, "done\nRemoval of small border surfaces by collapse... ");
		fflush(stdout);


		//Then, reload the original image

		//freeimage(image);


		ss_result2=list_newlist(MODE_FIFO);
		if(ss_result2==NULL)
		{
			fprintf(stderr, "Error: list_newlist() failed.\n");
			inventor_delete_scene(scene);
			freeimage(image);
			return(-1);
		}


		//complexe_free_complexe(vertex);
		//intersect_edges=list_pop(ss_result); //All the intersecting edges...keep for later
		//complexe_free_complexe(intersect_edges);

		inhibit=copyimage(image);

		//Then, scan all the list of complex, and remove, from inhibit, the ones which are under the threshold
		while(!list_isempty(ss_result))
		{
			temp=list_pop(ss_result);
			if(temp->num_fxy + temp->num_fxz + temp->num_fyz < lim)
			{
				for(i=0; i<temp->num_fxy; i++)
					cca_remove_complexface(inhibit, CC_FXY, temp->tab_fxy[i], rs, ps);

				for(i=0; i<temp->num_fyz; i++)
					cca_remove_complexface(inhibit, CC_FYZ, temp->tab_fyz[i], rs, ps);

				for(i=0; i<temp->num_fxz; i++)
					cca_remove_complexface(inhibit, CC_FXZ, temp->tab_fxz[i], rs, ps);
			}

			list_push(ss_result2, (void*)temp);
		}

		list_delete(ss_result, NO_FREE_DATA);
		ss_result=ss_result2;
		cca_makecomplex(inhibit);

		cca_collapse(image, inhibit, 0);

		freeimage(inhibit);
	}



	//Finally, print out the surfaces which still exist after the collapse
	fprintf(stdout, "done\nPrinting output surfaces... ");
	fflush(stdout);


	i=0;
	//while there is surfaces to write in output file
	while(!list_isempty(ss_result))
	{
		//Get the object to write
		temp=list_pop(ss_result);
		//if(temp->num_fxy + temp->num_fxz + temp->num_fyz >= lim)
			//Does the surface still exist in the image?
			if( i==0 ||
				((temp->num_fxy>0 && (UCHARDATA(image)[temp->tab_fxy[0]] & CC_FXY)!=0) ||
				(temp->num_fxz>0 && (UCHARDATA(image)[temp->tab_fxz[0]] & CC_FXZ)!=0) ||
				(temp->num_fyz>0 && (UCHARDATA(image)[temp->tab_fyz[0]] & CC_FYZ)!=0))	)
			{
				//Create an output file for the surface
				sprintf(name, "%s.surf%d.%d.iv", argv[2], temp->num_fxy+temp->num_fyz+temp->num_fxz, i);
				output = fopen(name, "wb");
				if(output==NULL)
				{
					fprintf(stderr, "Error: could not create output file %s.\n", name);
					freeimage(image);
					list_delete(ss_result, FREE_DATA);
					inventor_delete_scene(scene);
					return(-1);
				}

				//Link the output with the scene
				scene->output=output;

				//And write the complex object in the scene
				inventor_init_file(scene);
				if(i==0)
				{
					temp->tab_pt=vertex->tab_pt;
					temp->num_pt=vertex->num_pt;
				}
				else
					complexe_compute_vertex_array(temp, rs, ps);
				inventor_new_object(scene);
				//The first file to be written is edges of surfaces intersection... make them wide
				inventor_set_drawstyle(scene, FILLED, 1, 1);
				complexe_to_inventor(scene, temp, rs, ps, mode_amira);
				inventor_close_object(scene);

				fflush(scene->output);
				fclose(scene->output);
				i++;

			}

		complexe_free_complexe(temp);
	}

	fprintf(stdout, "done (%d surfaces)\n", i);



	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	freeimage(image);
	list_delete(ss_result, NO_FREE_DATA);


	return(0);
}


