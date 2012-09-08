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
	struct xvimage *in, *out;
	uint32_t rs, cs, d, x, y, z;

	if(argc!=3)
	{
		fprintf(stdout, "%s %s\n", argv[0], USAGE);
		return(0);
	}

	in=readimage(argv[1]);
	if(in==NULL)
	{
		fprintf(stderr, "Error, could not read %s\n", argv[1]);
		return(-1);
	}
	if(datatype(in)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte images supported\n");
		return(-1);
	}

	rs = rowsize(in);
	cs = colsize(in);
	d = depth(in);

	out=allocimage(NULL, rs+2, cs+2, d+2, VFF_TYP_1_BYTE);
	if(out==NULL)
	{
		fprintf(stderr, "Error, memory allocation failed.\n");
		return(-1);
	}


	for(z=0; z<d; z++)
		for(y=0; y<cs; y++)
			for(x=0; x<rs; x++)
				UCHARDATA(out)[(z+1)*(rs+2)*(cs+2) + (y+1)*(rs+2) + (x+1)] = UCHARDATA(in)[z*rs*cs + y*rs + x];

	writeimage(out, argv[2]);

	freeimage(in);
	freeimage(out);

	return(0);
}

