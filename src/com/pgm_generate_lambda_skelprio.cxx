/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>
#include <mcgeo.h>
#include <ldist.h>

//Bibliotheque perso


#define USAGE "<pgm_image> <lambdamap> <output_prio>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *lambdamap, *dist;
	uint32_t i, N, maxi;
	float seuil;

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
		fprintf(stderr, "Error: only 1 byte per pixel PGM image supported as input\n");
		freeimage(image);
		return(-1);
	}



	//We read lambdamap
	lambdamap=readimage(argv[2]);
	if (lambdamap==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		freeimage(image);
		return(-1);
	}
	else if(datatype(lambdamap)!=VFF_TYP_FLOAT)
	{
		fprintf(stderr, "Error: only float PGM image supported as lambdamap\n");
		freeimage(image);
		freeimage(lambdamap);
		return(-1);
	}
	else if (rowsize(image)!=rowsize(lambdamap) || colsize(image)!=colsize(lambdamap) || depth(image)!=depth(lambdamap))
	{
		fprintf(stderr, "Error: lambdamap image must have same dimensions as input\n");
		freeimage(image);
		freeimage(lambdamap);
		return(-1);
	}


	//*******************************************************
	//Conversion of image in cca format...
	//*******************************************************

	N=rowsize(image)*colsize(image)*depth(image);

	dist=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(dist==NULL)
	{
		fprintf(stderr, "Memory allocation error (1).\n");
		freeimage(image);
		freeimage(lambdamap);
		return(-1);
	}

	if (! lsedt_meijster(image, dist))
    {
		fprintf(stderr, "%s: lsedt_meijster failed\n", argv[0]);
		freeimage(image);
		freeimage(lambdamap);
		freeimage(dist);
		return(-1);
    }

    maxi=0;

	for(i=0; i<N; i++)
	{
		if(ULONGDATA(dist)[i]>maxi)
		{
			maxi=ULONGDATA(dist)[i];
		}
	}

	seuil=6.0;

	for(i=0; i<N; i++)
	{
		if(FLOATDATA(lambdamap)[i] >= seuil)
			FLOATDATA(lambdamap)[i] += maxi+1.0;
		else
			FLOATDATA(lambdamap)[i] = ULONGDATA(dist)[i];
	}

	//And write result to output
	writeimage(lambdamap, argv[3]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(lambdamap);
	freeimage(dist);


	return(0);
}

