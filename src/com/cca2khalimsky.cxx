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

#define USAGE "<cca_image> <output_khalimsky>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *cca_image, *result;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//We read input image
	cca_image=readimage(argv[1]);
	if (cca_image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(cca_image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only 1 byte per pixel image supported: is this a cca image?\n");
		return(-1);
	}
	else if(depth(cca_image)<=1)
	{
		fprintf(stderr, "Error: only 3D cca images supported\n");
		return(-1);
	}


	//*******************************************************
	//Conversion of image in cca format...
	//*******************************************************

	//We convert the image to cca format
	result=cca_to_khalimsky3d(cca_image);
	freeimage(cca_image);
	if(result==NULL)
	{
		fprintf(stderr, "Error: cca_to_khalimsky() failed.\n");
		return(-1);
	}

	//And write result to output
	writeimage(result, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(result);


	return(0);
}

