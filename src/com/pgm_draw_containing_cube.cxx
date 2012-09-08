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


#define USAGE "<in.pgm> <out.pgm>"

int main(int argc, char* argv[])
{
	int32_t N, i, j, k, rs, cs, ps, d, minx, maxx, miny, maxy, minz, maxz, n;
	struct xvimage *input;

	if(argc!=3)
	{
		fprintf(stderr, "%s %s\n", argv[0], USAGE);
		return(1);
	}

	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(input)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: Input must be byte image.\n");
		freeimage(input);
		return(-1);
	}

	rs=rowsize(input);
	cs=colsize(input);
	d=depth(input);
	ps=rs*cs;
	N=rs*cs*d;

	maxx=0;
	maxy=0;
	maxz=0;
	minx=rs;
	miny=cs;
	minz=d;

	n=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(UCHARDATA(input)[n++]!=0)
				{
					if(i>maxx) maxx=i;
					if(j>maxy) maxy=j;
					if(k>maxz) maxz=k;
					if(i<minx) minx=i;
					if(j<miny) miny=j;
					if(k<minz) minz=k;
				}
			}

	if(maxz>=minz && maxy>=miny && maxx>=minx)
	{
		for(k=minz; k<=maxz; k++)
			for(j=miny; j<=maxy; j++)
				for(i=minx; i<=maxx; i++)
				{
					UCHARDATA(input)[i+j*rs+k*ps]=255;
				}
	}



	writeimage(input, argv[2]);
	freeimage(input);
	return(0);
}

