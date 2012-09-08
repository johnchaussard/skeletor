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


#define USAGE "<source.pgm> <destination.pgm>"

int main(int argc, char* argv[])
{
	uint32_t in_rs, in_ps, out_rs, out_ps, limx, limy, limz, i, j, k;
	struct xvimage *in, *out;

	if(argc!=3)
	{
		fprintf(stderr, "%s %s\n", argv[0], USAGE);
		return(1);
	}

	in=readimage(argv[1]);
	if(in==NULL)
	{
		fprintf(stderr, "Image %s could not be found.\n", argv[1]);
		return(1);
	}
	in_rs=rowsize(in); in_ps=colsize(in)*in_rs;

	out=readimage(argv[2]);
	if(out==NULL)
	{
		fprintf(stderr, "Image %s could not be found.\n", argv[2]);
		freeimage(in);
		return(1);
	}
	out_rs=rowsize(out); out_ps=colsize(out)*out_rs;

	limx=rowsize(in);
	if((uint32_t)rowsize(out) < limx) limx=rowsize(out);

	limy=colsize(in);
	if((uint32_t)colsize(out) < limy) limy=colsize(out);

	limz=depth(in);
	if((uint32_t)depth(out) < limz) limz=depth(out);


	if(datatype(in)!=datatype(out))
	{
		fprintf(stderr, "Both images must have the same type.\n");
		freeimage(in);
		freeimage(out);
		return(1);
	}


	switch(datatype(in))
	{
		case(VFF_TYP_1_BYTE):
			for(k=0; k<limz; k++)
			for(j=0; j<limy; j++)
			for(i=0; i<limx; i++)
				UCHARDATA(out)[i+j*out_rs+k*out_ps]=UCHARDATA(in)[i+j*in_rs+k*in_ps];
			break;

		case(VFF_TYP_2_BYTE):
			for(k=0; k<limz; k++)
			for(j=0; j<limy; j++)
			for(i=0; i<limx; i++)
				USHORTDATA(out)[i+j*out_rs+k*out_ps]=USHORTDATA(in)[i+j*in_rs+k*in_ps];
			break;

		case(VFF_TYP_4_BYTE):
			for(k=0; k<limz; k++)
			for(j=0; j<limy; j++)
			for(i=0; i<limx; i++)
				ULONGDATA(out)[i+j*out_rs+k*out_ps]=ULONGDATA(in)[i+j*in_rs+k*in_ps];
			break;

		case(VFF_TYP_FLOAT):
			for(k=0; k<limz; k++)
			for(j=0; j<limy; j++)
			for(i=0; i<limx; i++)
				FLOATDATA(out)[i+j*out_rs+k*out_ps]=FLOATDATA(in)[i+j*in_rs+k*in_ps];
			break;

		case(VFF_TYP_DOUBLE):
			for(k=0; k<limz; k++)
			for(j=0; j<limy; j++)
			for(i=0; i<limx; i++)
				DOUBLEDATA(out)[i+j*out_rs+k*out_ps]=DOUBLEDATA(in)[i+j*in_rs+k*in_ps];
			break;

		default:
			fprintf(stderr, "Image type not supported.\n");
			freeimage(in);
			freeimage(out);
			return(1);
	}


	freeimage(in);
	writeimage(out, argv[2]);
	freeimage(out);
	return(0);
}
