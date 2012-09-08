/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<input_cc> <output_cc>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
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
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}


	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We compute the surface axis
	if(cca_vsa_collapse_keep_faces(image)<0)
	{
		fprintf(stderr, "Error: cca_compute_surface_axis() failed.\n");
		freeimage(image);
		return(-1);
	}



	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);


	return(0);
}
