/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>


#define USAGE "<in.pgm> <out.pgm>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *input, *output;
	uint32_t i, N;
	if(argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}

	N=rowsize(input)*colsize(input)*depth(input);

	if(datatype(input) == VFF_TYP_DOUBLE)
	{
		fprintf(stdout, "Input is already a double image.\n");
		freeimage(input);
		return 0;
	}

	output=allocimage(NULL, rowsize(input), colsize(input), depth(input), VFF_TYP_DOUBLE);
	if(output==NULL)
	{
		fprintf(stderr, "Could not create output image.\n");
		freeimage(input);
		return(-1);
	}

	switch(datatype(input))
	{
		case(VFF_TYP_1_BYTE):
			fprintf(stdout, "Warning, assuming input image is unsigned.\n");
			for(i=0; i<N; i++)
				DOUBLEDATA(output)[i]=(double)UCHARDATA(input)[i];
			break;

		case(VFF_TYP_2_BYTE):
			fprintf(stdout, "Warning, assuming input image is unsigned.\n");
			for(i=0; i<N; i++)
				DOUBLEDATA(output)[i]=(double)USHORTDATA(input)[i];
			break;

		case(VFF_TYP_4_BYTE):
			fprintf(stdout, "Warning, assuming input image is unsigned.\n");
			for(i=0; i<N; i++)
				DOUBLEDATA(output)[i]=(double)ULONGDATA(input)[i];
			break;

		case(VFF_TYP_FLOAT):
			for(i=0; i<N; i++)
				DOUBLEDATA(output)[i]=(double)FLOATDATA(input)[i];
			break;

		default:
			fprintf(stderr, "Incorrect input image type.\n");
			freeimage(input);
			freeimage(output);
			return(-1);
			break;
	}

	writeimage(output, argv[2]);

	return(0);
}

