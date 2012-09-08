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

#define USAGE "<khalimsky_image> <output_cca>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *khal_image, *result;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//We read input image
	khal_image=readimage(argv[1]);
	if (khal_image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(khal_image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only 1 byte per pixel PGM image supported\n");
		return(-1);
	}


	//*******************************************************
	//Conversion of image in cca format...
	//*******************************************************

	//We convert the image to cca format
	result=khalimsky3d_to_cca(khal_image);
	freeimage(khal_image);
	if(result==NULL)
	{
		fprintf(stderr, "Error: khalimsky3d_to_cca() failed.\n");
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

