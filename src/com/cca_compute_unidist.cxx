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



int main(int argc, char* argv[])
{
	uint32_t ps, N, on;
	int i, j, k, rs, cs, d;
	struct xvimage *image, *out;

	image=readimage(argv[1]);
	if(image==NULL)
	{
		fprintf(stderr, "Could not read image %s.\n", argv[1]);
		return(-1);
	}


	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=ps*d;

	out=allocimage(NULL, rs, cs, d, VFF_TYP_4_BYTE);

	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
		{
			on=0;
			for(i=0; i<rs; i++)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
			on=0;
			for(i=rs-1; i>=0; i--)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
		}

	for(k=0; k<d; k++)
		for(i=0; i<rs; i++)
		{
			on=0;
			for(j=0; j<cs; j++)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
			on=0;
			for(j=cs-1; j>=0; j--)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
		}

	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			on=0;
			for(k=0; k<d; k++)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
			on=0;
			for(k=d-1; k>=0; k--)
			{
				if(UCHARDATA(image)[i+j*rs+k*ps] != 0)
				{
					on++;
					if(ULONGDATA(out)[i+j*rs+k*ps] == 0 || ULONGDATA(out)[i+j*rs+k*ps] > on)
						ULONGDATA(out)[i+j*rs+k*ps]=on;
				}
				else
					on=0;
			}
		}

	freeimage(image);
	writeimage(out, argv[2]);
	freeimage(out);
	return(0);
}
