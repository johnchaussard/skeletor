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

#define USAGE "<input.cc> <container.cc> <inhibit.cc|NULL> <output.cc>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *input, *inhibit, *container;
	if(argc!=5)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the image
	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(input)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input (cc images).\n");
		freeimage(input);
		return(-1);
	}


	//Then, open the propagation image
	container=readimage(argv[2]);
	if(container==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[2]);
		freeimage(input);
		return(-1);
	}
	if(datatype(container)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for container (cc images).\n");
		freeimage(input);
		freeimage(container);
		return(-1);
	}

	if(rowsize(input)!=rowsize(container) || colsize(input)!=colsize(container) || depth(input)!=depth(container))
	{
		fprintf(stderr, "Input images must have same size (container and input don't).\n");
		freeimage(input);
		freeimage(container);
		return(-1);
	}


	//Finally, open the inhibit image if there is one
	if(strcmp(argv[3], "NULL")!=0)
	{
		inhibit=readimage(argv[3]);
		if(inhibit==NULL)
		{
			fprintf(stderr, "Error, could not open %s\n", argv[3]);
			freeimage(input);
			freeimage(container);
			return(-1);
		}
		if(datatype(inhibit)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error, only one byte image supported for inhibit (cc images).\n");
			freeimage(input);
			freeimage(container);
			freeimage(inhibit);
			return(-1);
		}

		if(rowsize(input)!=rowsize(inhibit) || colsize(input)!=colsize(inhibit) || depth(input)!=depth(inhibit))
		{
			fprintf(stderr, "Input images must have same size (container and input don't).\n");
			freeimage(input);
			freeimage(container);
			freeimage(inhibit);
			return(-1);
		}
	}
	else
		inhibit=NULL;


	if(cca_geodilat(input, container, inhibit)<0)
	{
		fprintf(stderr, "Error while performing geodesic dilatation.\n");
		freeimage(input);
		freeimage(container);
		freeimage(inhibit);
		return(-1);
	}

	if(inhibit!=NULL)
		freeimage(inhibit);
	freeimage(container);

	writeimage(input, argv[4]);

	freeimage(input);

	return(0);
}

