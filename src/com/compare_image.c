//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Librairie perso
#include <libcomplex.h>
#include <libcollapse.h>
#include <libinventor.h>


int32_t main(int argc, char *argv[])
{
	struct xvimage *image1, *image2;
	uint32_t rs, cs, d, n, i, j, k;

	image1=readimage(argv[1]);
	image2=readimage(argv[2]);

	if(rowsize(image1)!=rowsize(image2))
	{
		fprintf(stdout, "Pictures don't have the same rowsize.\n");
		return(0);
	}

	if(colsize(image1)!=colsize(image2))
	{
		fprintf(stdout, "Pictures don't have the same colsize.\n");
		return(0);
	}

	if(depth(image1)!=depth(image2))
	{
		fprintf(stdout, "Pictures don't have the same depth.\n");
		return(0);
	}

	rs=rowsize(image1);
	cs=colsize(image1);
	d=depth(image1);

	n=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(UCHARDATA(image1)[n]!=UCHARDATA(image2)[n])
				{
					fprintf(stdout, "Pixel x=%d  y=%d  z=%d  (%d) is not the same (%d and %d).\n", i, j, k, n, UCHARDATA(image1)[n], UCHARDATA(image2)[n]);
					//return(0);
				}
				n++;
			}

	return(0);
}
