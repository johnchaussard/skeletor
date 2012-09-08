/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>


#define USAGE "<in.pgm> <factor> <out.pgm>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *image;;
	uint32_t i, N;
	float divfactor;
	if(argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the image
	image=readimage(argv[1]);
	if(image==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input.\n");
		return(-1);
	}

	divfactor=atof(argv[2]);

	N=rowsize(image)*colsize(image)*depth(image);

	for(i=0; i<N; i++)
		UCHARDATA(image)[i]=(unsigned char)(UCHARDATA(image)[i] / divfactor);

	writeimage(image, argv[3]);

	return(0);
}

