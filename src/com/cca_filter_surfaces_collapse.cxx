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

#define USAGE "<input_cca> <num_loop_collapse> <output_cca>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	list *ss_result;
	complexe *temp;
	uint32_t i, erode, cpt, keep;



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
		freeimage(image);
		return(-1);
	}

	erode=atoi(argv[2]);
	if(erode<=0)
	{
		fprintf(stdout,"Error: positive integer necessary for parameter.\n");
		freeimage(image);
		return(-1);
	}




	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);




	//********************************************************
	//Make surface segmentation
	//********************************************************

	ss_result=cca_simple_surface_segmentation(image);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation() failed.\n");
		freeimage(image);
		return(-1);
	}


	//*******************************************************
	//Collapse original image
	//*******************************************************

	//Restore original image
	freeimage(image);
	image=readimage(argv[1]);

	//Performs collapse
	if( cca_collapse_method_facesandsubfaces2(image, NULL, erode)<0 )
	{
		fprintf(stderr, "Error: cca_collapse_method_facesandsubfaces2() failed.\n");
		freeimage(image);
		while(!list_isempty(ss_result))
		{
			temp=(complexe*)list_pop_pointer(ss_result);
			complexe_free_complexe(temp);
		}
		list_delete(ss_result, FREE_DATA);
		return(-1);
	}



	//****************************************************
	//Restore in image surfaces which have not been completly deleted (still one face survived)
	//****************************************************

	//We don't care the two first items of the list as they are vertex and edges...
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);


	cpt=0;
	//For each surface
	while(!list_isempty(ss_result))
	{
		keep=0;
		temp=(complexe*)list_pop_pointer(ss_result);

		//try to find a face of the surface which may have survived in image
		for(i=0; i<temp->num_fxy && keep==0; i++)
			if( (UCHARDATA(image)[temp->tab_fxy[i]] & CC_FXY) != 0)
				keep=1;

		for(i=0; i<temp->num_fxz && keep==0; i++)
			if( (UCHARDATA(image)[temp->tab_fxz[i]] & CC_FXZ) != 0)
				keep=1;

		for(i=0; i<temp->num_fyz && keep==0; i++)
			if( (UCHARDATA(image)[temp->tab_fyz[i]] & CC_FYZ) != 0)
				keep=1;

		if(keep==1)
			complexe_add_to_cca(image, temp);
		else
			cpt++;

		complexe_free_complexe(temp);
	}


	//**************************************
	//Finish
	//*************************************

	list_delete(ss_result, FREE_DATA);
	cca_makecomplex(image);
	writeimage(image, argv[3]);
	freeimage(image);

	fprintf(stdout, "%d surfaces removed.\n", cpt);

	return(0);
}


