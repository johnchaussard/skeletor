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

#define USAGE "<input_image.cc> <output_cube.cc>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *cc_mask;
	uint32_t i, ccps, ccN, j, k, pix, ccrs, cccs, ccd;

	if(argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the inhibit image
	cc_mask=readimage(argv[1]);
	if(cc_mask==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(cc_mask)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input.\n");
		return(-1);
	}

	ccrs=rowsize(cc_mask);
	cccs=colsize(cc_mask);
	ccps=ccrs*cccs;
	ccd=depth(cc_mask);
	ccN=ccps*ccd;

	for(i=0; i<ccN; i++)
	{
		UCHARDATA(cc_mask)[i]=0;
	}

	//Keep only the faces which are on border
	pix=0;
	for(k=0; k<ccd; k++)
		for(j=0; j<cccs; j++)
			for(i=0; i<ccrs; i++)
			{
				if(cca_is_face_onborder(cc_mask, CC_FXY, i, j, k)!=0 && i<ccrs-1 && j<cccs-1)
					UCHARDATA(cc_mask)[pix]|=CC_FXY;

				if(cca_is_face_onborder(cc_mask, CC_FXZ, i, j, k)!=0 && i<ccrs-1 && k<ccd-1)
					UCHARDATA(cc_mask)[pix]|=CC_FXZ;

				if(cca_is_face_onborder(cc_mask, CC_FYZ, i, j, k)!=0 && k<ccd-1 && j<cccs-1)
					UCHARDATA(cc_mask)[pix]|=CC_FYZ;

				pix++;
			}


	cca_makecomplex(cc_mask);

	writeimage(cc_mask, argv[2]);

	return(0);
}





