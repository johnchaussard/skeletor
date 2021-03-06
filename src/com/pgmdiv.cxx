/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>


//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<in1.pgm> <in2.pgm> <out.pgm>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *image1, *image2;
	uint32_t i, N;
	if(argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	image1=readimage(argv[1]);
	if(image1==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(image1)!=VFF_TYP_FLOAT)
	{
		fprintf(stderr, "Error, only float image supported for input.\n");
		return(-1);
	}

	//First, open the inhibit image
	image2=readimage(argv[2]);
	if(image2==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[2]);
		return(-1);
	}
	if(datatype(image2)!=VFF_TYP_FLOAT)
	{
		fprintf(stderr, "Error, only float image supported for input.\n");
		return(-1);
	}

	if(rowsize(image1)!=rowsize(image2) || colsize(image1)!=colsize(image2) || depth(image1)!=depth(image2))
	{
		fprintf(stderr, "Both input images must have same size.\n");
		return(-1);
	}

	N=rowsize(image1)*colsize(image1)*depth(image1);

	for(i=0; i<N; i++)
		if (ULONGDATA(image2)[i] !=0)
			FLOATDATA(image1)[i]=FLOATDATA(image1)[i]/(float)FLOATDATA(image2)[i];
		else
			FLOATDATA(image1)[i]=0.0;

	writeimage(image1, argv[3]);

	return(0);
}

