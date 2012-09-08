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

#define USAGE "<input_distmap> <factor> <output_distmap>"


int main(int argc, char* argv[])
{
	struct xvimage *distmap_in, *distmap_out, *temp;
	uint32_t i, j, k, rs, cs, d, ps, N, t, pix, factor;

	if(argc!=4)
	{
		fprintf(stdout, "%s\n", USAGE);
		return(-1);
	}

	distmap_in=readimage(argv[1]);
	if(distmap_in==NULL)
	{
		fprintf(stderr, "Could not read image %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(distmap_in)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Distance map must be a long image.\n");
		return(-1);
	}

	factor=atoi(argv[2]);

	rs=rowsize(distmap_in);
	cs=colsize(distmap_in);
	d=depth(distmap_in);
	ps=rs*cs;
	N=ps*d;

	//Allocate memory for output image
	distmap_out=allocimage(NULL, rs, cs, d, VFF_TYP_4_BYTE);
	if(distmap_out==NULL)
	{
		fprintf(stderr, "Memory allocation error for output image.\n");
		return(-1);
	}

	t=0;
	while(t<factor)
	{
		pix=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if(ULONGDATA(distmap_in)[pix]>=t)
					{
						if(ULONGDATA(distmap_in)[pix]>ULONGDATA(distmap_out)[pix])
							ULONGDATA(distmap_out)[pix]=ULONGDATA(distmap_in)[pix];

						if(i>0 && (ULONGDATA(distmap_in)[pix-1]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix-1]=ULONGDATA(distmap_in)[pix];

						if(i<rs-1 && (ULONGDATA(distmap_in)[pix+1]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix+1]=ULONGDATA(distmap_in)[pix];

						if(j>0 && (ULONGDATA(distmap_in)[pix-rs]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix-rs]=ULONGDATA(distmap_in)[pix];

						if(j<cs-1 && (ULONGDATA(distmap_in)[pix+rs]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix+rs]=ULONGDATA(distmap_in)[pix];

						if(k>0 && (ULONGDATA(distmap_in)[pix-ps]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix-ps]=ULONGDATA(distmap_in)[pix];

						if(k<d-1 && (ULONGDATA(distmap_in)[pix+ps]<ULONGDATA(distmap_in)[pix]))
							ULONGDATA(distmap_out)[pix+ps]=ULONGDATA(distmap_in)[pix];
					}

					pix++;
				}
		t++;
		temp=distmap_in;
		distmap_in=distmap_out;
		distmap_out=temp;
		fprintf(stdout, "Tour %d\n", t);
	}

	freeimage(distmap_out);
	writeimage(distmap_in, argv[3]);
	freeimage(distmap_in);
	return(0);
}

