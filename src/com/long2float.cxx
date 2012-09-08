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


#define USAGE "<in.pgm> <out.pgm>"

int main(int argc, char* argv[])
{
	int32_t N,i;
	struct xvimage *input, *output;

	if(argc!=3)
	{
		fprintf(stderr, "%s %s\n", argv[0], USAGE);
		return(1);
	}

	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(input)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: Input must be long image.\n");
		freeimage(input);
		return(-1);
	}

	output=allocimage(NULL, rowsize(input), colsize(input), depth(input), VFF_TYP_FLOAT);
	if(output==NULL)
	{
		fprintf(stderr, "Error: Could not allocate output image.\n");
		freeimage(input);
		return(-1);
	}

	N=rowsize(input)*colsize(input)*depth(input);
	for(i=0; i<N; i++)
	{
		FLOATDATA(output)[i]=(float)ULONGDATA(input)[i];
	}


	writeimage(output, argv[2]);
	freeimage(input);
	freeimage(output);
	return(0);
}
