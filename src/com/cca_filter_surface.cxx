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

#define USAGE "<cca_input_image> <cca_output_image> <min_size_to_keep>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	uint32_t size, i, N;
	complexe *temp;
	list *surfaces_to_remove;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=4)
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

	size=-1;
	size=atoi(argv[3]);
	if(size<0)
	{
		fprintf(stderr, "Error: positive size value required.\n");
		return(-1);
	}

	N=rowsize(image)*colsize(image)*depth(image);


	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);



	//********************************************************
	//First, detect the surface intersections and remove them
	//********************************************************

	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): could not create new complexe (2).\n");
		return(-1);
	}

	if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 3)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while scanning for surfaces inersections.\n");
		return(-1);
	}

	if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 4)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while scanning for surfaces inersections.\n");
		return(-1);
	}

	//Remove the surfaces intersections
	for(i=0; i<temp->num_ax; i++)
	{
		UCHARDATA(image)[temp->tab_ax[i]]=(UCHARDATA(image)[temp->tab_ax[i]]|CC_AX)-CC_AX;
	}
	for(i=0; i<temp->num_ay; i++)
	{
		UCHARDATA(image)[temp->tab_ay[i]]=(UCHARDATA(image)[temp->tab_ay[i]]|CC_AY)-CC_AY;
	}
	for(i=0; i<temp->num_az; i++)
	{
		UCHARDATA(image)[temp->tab_az[i]]=(UCHARDATA(image)[temp->tab_az[i]]|CC_AZ)-CC_AZ;
	}

	//Free the temp structure
	complexe_free_complexe(temp);


	//********************************************************
	//Then, detect the surfaces and keep the small ones, in order to remove them
	//********************************************************

	surfaces_to_remove=list_newlist(MODE_FIFO, 64);
	if(surfaces_to_remove==NULL)
	{
		fprintf(stderr, "Error, list creation failed.\n");
		return(-1);
	}

	//Then look for faces in the image
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_FXY) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): could not create new complexe (3).\n");
				return(-1);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXY)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while computing simple surface (1).\n");
				return(-1);
			}

			//If surface is small, keep it in memory
			if(temp->num_fxy + temp->num_fxz + temp->num_fyz < size)
				list_push_pointer(surfaces_to_remove, temp);
			else
				complexe_free_complexe(temp);
		}

		if( (UCHARDATA(image)[i] & CC_FXZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): could not create new complexe (3).\n");
				return(-1);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while computing simple surface (1).\n");
				return(-1);
			}

			//If surface is small, keep it in memory
			if(temp->num_fxy + temp->num_fxz + temp->num_fyz < size)
				list_push_pointer(surfaces_to_remove, temp);
			else
				complexe_free_complexe(temp);
		}

		if( (UCHARDATA(image)[i] & CC_FYZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): could not create new complexe (3).\n");
				return(-1);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FYZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_to_iv(): error while computing simple surface (1).\n");
				return(-1);
			}

			//If surface is small, keep it in memory
			if(temp->num_fxy + temp->num_fxz + temp->num_fyz < size)
				list_push_pointer(surfaces_to_remove, temp);
			else
				complexe_free_complexe(temp);
		}
	}


	//********************************************************
	//Remove the small surfaces... We need to open again the image, to restore all data, and then remove small surfaces
	//********************************************************
	if(!list_isempty(surfaces_to_remove))
	{

		//Close and open again the image
		freeimage(image);
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

		while(!list_isempty(surfaces_to_remove))
		{
			temp=(complexe*)list_pop_pointer(surfaces_to_remove);
			//Remove the detected faces
			for(i=0; i<temp->num_fxy; i++)
			{
				UCHARDATA(image)[temp->tab_fxy[i]]=(UCHARDATA(image)[temp->tab_fxy[i]]|CC_FXY)-CC_FXY;
			}
			for(i=0; i<temp->num_fxz; i++)
			{
				UCHARDATA(image)[temp->tab_fxz[i]]=(UCHARDATA(image)[temp->tab_fxz[i]]|CC_FXZ)-CC_FXZ;
			}
			for(i=0; i<temp->num_fyz; i++)
			{
				UCHARDATA(image)[temp->tab_fyz[i]]=(UCHARDATA(image)[temp->tab_fyz[i]]|CC_FYZ)-CC_FYZ;
			}

			complexe_free_complexe(temp);
		}

		//*****************************************************
		//Make a nice output image
		//*****************************************************
		//We remove all points, volumes, and edges
		for(i=0; i<N; i++)
		{
			UCHARDATA(image)[i] = (UCHARDATA(image)[i]&(CC_FXY|CC_FYZ|CC_FXZ));
		}

		cca_makecomplex(image);
	}

	writeimage(image, argv[2]);


	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	list_delete(surfaces_to_remove, FREE_DATA);


	return(0);
}


