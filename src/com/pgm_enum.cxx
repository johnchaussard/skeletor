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

#define USAGE "<input_image.pgm>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *input;
	uint32_t rs, N, i;

	if(argc!=2)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//First, open the image
	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(1);
	}

	rs=rowsize(input);
	N=rs*colsize(input);

	for(i=0; i<N; i++)
	{
		fprintf(stdout, "%d %d %f\n", i%rs, i/rs, FLOATDATA(input)[i]);
	}


	freeimage(input);

	return(0);
}






