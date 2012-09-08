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

#define USAGE "<in.cc> <out.cc>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *image1;
	uint32_t i, N, rs, cs, d;
	if(argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the inhibit image
	image1=readimage(argv[1]);
	if(image1==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(image1)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input (cc images).\n");
		return(-1);
	}

	rs=rowsize(image1);
	cs=colsize(image1);
	d=depth(image1);
	N=rs*cs*d;

	for(i=0; i<N; i++)
		UCHARDATA(image1)[i]&=255-(CC_VOL | CC_FXY | CC_FYZ | CC_FXZ | CC_PT);

	cca_makecomplex(image1);


	writeimage(image1, argv[2]);

	return(0);
}


