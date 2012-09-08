/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

#define USAGE "<input_distmap> <output_distmap>"


int main(int argc, char* argv[])
{
	struct xvimage *distmap_in, *distmap_out, *temp;
	uint32_t i, j, rs, cs, N, t, pix;
	float max;

	if(argc!=3)
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
	else if(datatype(distmap_in)!=VFF_TYP_FLOAT)
	{
		fprintf(stderr, "Distance map must be a float image.\n");
		return(-1);
	}

	rs=rowsize(distmap_in);
	cs=colsize(distmap_in);
	N=rs*cs;

	//Allocate memory for output image
	distmap_out=allocimage(NULL, rs, cs, 1, VFF_TYP_FLOAT);
	if(distmap_out==NULL)
	{
		fprintf(stderr, "Memory allocation error for output image.\n");
		return(-1);
	}

	max=FLOATDATA(distmap_in)[0];
	for(i=1; i<N; i++)
	{
		if(FLOATDATA(distmap_in)[i]>max)
			max=FLOATDATA(distmap_in)[i];
	}

	fprintf(stdout, "max value: %f\n", max);


	t=0;
	while(t<ceilf(max))
	{
		pix=0;
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if(FLOATDATA(distmap_in)[pix]>=2*t)
					{
						if(FLOATDATA(distmap_in)[pix]>FLOATDATA(distmap_out)[pix])
							FLOATDATA(distmap_out)[pix]=FLOATDATA(distmap_in)[pix];

						if(i>0 && (FLOATDATA(distmap_in)[pix-1]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix-1]=FLOATDATA(distmap_in)[pix];

						if(i<rs-1 && (FLOATDATA(distmap_in)[pix+1]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix+1]=FLOATDATA(distmap_in)[pix];

						if(j>0 && (FLOATDATA(distmap_in)[pix-rs]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix-rs]=FLOATDATA(distmap_in)[pix];

						if(j<cs-1 && (FLOATDATA(distmap_in)[pix+rs]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix+rs]=FLOATDATA(distmap_in)[pix];

						if(i>0 && j>0 && (FLOATDATA(distmap_in)[pix-rs-1]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix-rs-1]=FLOATDATA(distmap_in)[pix];

						if(i>0 && j<cs-1 && (FLOATDATA(distmap_in)[pix+rs-1]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix+rs-1]=FLOATDATA(distmap_in)[pix];

						if(i<rs-1 && j>0 && (FLOATDATA(distmap_in)[pix+1-rs]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix+1-rs]=FLOATDATA(distmap_in)[pix];

						if(i<rs-1 && j<cs-1 && (FLOATDATA(distmap_in)[pix+rs+1]<FLOATDATA(distmap_in)[pix]))
							FLOATDATA(distmap_out)[pix+rs+1]=FLOATDATA(distmap_in)[pix];
					}

					pix++;
				}
		t++;
		temp=distmap_in;
		distmap_in=distmap_out;
		distmap_out=temp;
	}

	freeimage(distmap_out);
	distmap_out=readimage(argv[1]);

	writeimage(distmap_out, argv[2]);

	freeimage(distmap_in);
	freeimage(distmap_out);
	return(0);
}

