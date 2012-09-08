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
	uint32_t i, j, k, N, pix, t, rs, cs, d;
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

	pix=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if( (UCHARDATA(image1)[pix] & CC_AX) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AX, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AX);
				}

				if( (UCHARDATA(image1)[pix] & CC_AY) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AY, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AY);
				}

				if( (UCHARDATA(image1)[pix] & CC_AZ) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AZ, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AZ);
				}

				pix++;
			}

	for(i=0; i<N; i++)
		UCHARDATA(image1)[i]&=255-(CC_VOL | CC_FXY | CC_FYZ | CC_FXZ | CC_PT);

	cca_makecomplex(image1);


	writeimage(image1, argv[2]);

	return(0);
}

