/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

#define USAGE "<in1.pgm> <in2.pgm>"

//Useful for lambda medial axis scripts

int32_t main(int argc, char* argv[])
{
	struct xvimage *image1, *image2;
	uint32_t i, N;
	unsigned long int a, b, max, mean, num_pix_non_nul;
	float fa, fb, fmax, fmean;
	double da, db, dmax, dmean;


	if(argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	image1=readimage(argv[1]);
	if(image1==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}

	image2=readimage(argv[2]);
	if(image2==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[2]);
		return(-1);
	}

	if(datatype(image2)!=datatype(image1))
	{
		fprintf(stderr, "Error, input images must have same type.\n");
		return(-1);
	}

	if(rowsize(image1)!=rowsize(image2) || colsize(image1)!=colsize(image2) || depth(image1)!=depth(image2))
	{
		fprintf(stderr, "Input images must have same size.\n");
		return(-1);
	}

	N=rowsize(image1)*colsize(image1)*depth(image1);


	switch(datatype(image1))
	{
		case(VFF_TYP_1_BYTE):
			num_pix_non_nul=0;
			max=0;
			mean=0;
			for(i=0; i<N; i++)
			{
				a=UCHARDATA(image1)[i];
				b=UCHARDATA(image2)[i];
				if(max<(uint32_t)labs(a-b)) max=labs(a-b);
				if(a!=0 || b!=0)
				{
					num_pix_non_nul++;
					mean+=labs(a-b);
				}
			}
			printf("max:%ld mean:%f\n", max, (double)mean/(double)num_pix_non_nul);
			break;

		case(VFF_TYP_2_BYTE):
			num_pix_non_nul=0;
			max=0;
			mean=0;
			for(i=0; i<N; i++)
			{
				a=USHORTDATA(image1)[i];
				b=USHORTDATA(image2)[i];
				if(max<(uint32_t)labs(a-b)) max=labs(a-b);
				if(a!=0 || b!=0)
				{
					num_pix_non_nul++;
					mean+=labs(a-b);
				}
			}
			printf("max:%ld mean:%f\n", max, (double)mean/(double)num_pix_non_nul);
			break;

		case(VFF_TYP_4_BYTE):
			num_pix_non_nul=0;
			max=0;
			mean=0;
			for(i=0; i<N; i++)
			{
				a=ULONGDATA(image1)[i];
				b=ULONGDATA(image2)[i];
				if(max<(uint32_t)labs(a-b)) max=labs(a-b);
				if(a!=0 || b!=0)
				{
					num_pix_non_nul++;
					mean+=labs(a-b);
				}
			}
			printf("max:%ld mean:%f\n", max, (double)mean/(double)num_pix_non_nul);
			break;

		case(VFF_TYP_FLOAT):
			num_pix_non_nul=0;
			fmax=0;
			fmean=0;
			for(i=0; i<N; i++)
			{
				fa=FLOATDATA(image1)[i];
				fb=FLOATDATA(image2)[i];
				if(fmax<fabsf(fa-fb)) fmax=fabsf(fa-fb);
				if(fa!=0.0 || fb!=0.0)
				{
					num_pix_non_nul++;
					fmean+=(unsigned long int)fabsf(fa-fb);
				}
			}
			printf("max:%f mean:%f\n", fmax, (double)fmean/(double)num_pix_non_nul);
			break;

		case(VFF_TYP_DOUBLE):
			num_pix_non_nul=0;
			dmax=0.0;
			dmean=0;
			for(i=0; i<N; i++)
			{
				da=DOUBLEDATA(image1)[i];
				db=DOUBLEDATA(image2)[i];
				if(dmax<fabs(da-db)) dmax=fabs(da-db);
				if(da!=0.0 || db!=0.0)
				{
					num_pix_non_nul++;
					mean+=(unsigned long int)fabs(da-db);
				}
			}
			printf("max:%f mean:%f\n", dmax, dmean/(double)num_pix_non_nul);
			break;

		default:
			fprintf(stderr, "Error, image type not supported.\n");
	}

	freeimage(image1);
	freeimage(image2);

	return(0);
}

