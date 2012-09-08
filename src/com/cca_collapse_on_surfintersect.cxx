/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<input_cc> <output_cc> <time_to_collapse>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *inhibit;
	uint32_t N, i, j, k, cs, ps, rs, d;
	int times;

	image=readimage(argv[1]);
	times=atoi(argv[2]);

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=0;

	inhibit=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);

	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				N=i+j*rs+k*ps;

				if((UCHARDATA(image)[N]&CC_AX) !=0)
					if(cca_count_face_container(image, N, i, j, k, CC_AX, rs, ps) >2)
						UCHARDATA(inhibit)[N] |= CC_AX;

				if((UCHARDATA(image)[N]&CC_AY) !=0)
					if(cca_count_face_container(image, N, i, j, k, CC_AY, rs, ps) >2)
						UCHARDATA(inhibit)[N] |= CC_AY;

				if((UCHARDATA(image)[N]&CC_AZ) !=0)
					if(cca_count_face_container(image, N, i, j, k, CC_AZ, rs, ps) >2)
						UCHARDATA(inhibit)[N] |= CC_AZ;

			}

	cca_collapse(image, inhibit, times);

	writeimage(image, argv[3]);

	freeimage(image);
	freeimage(inhibit);
	return(0);
}



