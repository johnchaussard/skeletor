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

#define USAGE "<input_image1.pgm> <input_image2.pgm> <output_image.pgm>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *input1, *input2;
	uint32_t N, i;

	if(argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//First, open the image
	input1=readimage(argv[1]);
	if(input1==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(1);
	}
	else if(datatype(input1)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error, input image should be int32 image.\n");
		freeimage(input1);
		return(1);
	}

	//Then, open the second image
	input2=readimage(argv[2]);
	if(input2==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		freeimage(input1);
		return(1);
	}
	else if(datatype(input2)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error, input image should be int32 image.\n");
		freeimage(input1);
		freeimage(input2);
		return(1);
	}


	if(rowsize(input1)!=rowsize(input2) || colsize(input1)!=colsize(input2) || depth(input1)!=depth(input2))
	{
		fprintf(stderr, "Error, images should be same size.\n");
		freeimage(input1);
		freeimage(input2);
		return(1);
	}

	N=rowsize(input1)*colsize(input1)*depth(input1);

	for(i=0; i<N; i++)
	{
		if(ULONGDATA(input1)[i] != ULONGDATA(input2)[i])
			ULONGDATA(input1)[i]=0;
	}


	writeimage(input1, argv[3]);
	freeimage(input1);
	freeimage(input2);


	return(0);
}






