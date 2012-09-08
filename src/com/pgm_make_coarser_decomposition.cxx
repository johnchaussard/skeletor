/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>


#define USAGE "<pgm_label_image1> <pgm_label_image2> <pgm_output_image>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *image2;
	uint32_t i, rs_pgm, cs_pgm, d_pgm, ps_pgm, N_pgm, max, *tab1, max2, *tab2;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(image)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: only integer PGM image supported\n");
		freeimage(image);
		return(1);
	}


	image2=readimage(argv[2]);
	if (image2==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		freeimage(image);
		return(1);
	}
	else if(datatype(image2)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: only integer PGM image supported\n");
		freeimage(image);
		freeimage(image2);
		return(1);
	}


	rs_pgm=rowsize(image);
	cs_pgm=colsize(image);
	d_pgm=depth(image);
	ps_pgm=rs_pgm*cs_pgm;
	N_pgm=ps_pgm*d_pgm;

	if(rowsize(image2)!=rowsize(image) || colsize(image2)!=colsize(image) || depth(image2)!=depth(image))
	{
		fprintf(stderr, "Error: Image's sizes must be same\n");
		freeimage(image);
		freeimage(image2);
		return(1);
	}

	max=0;
	for(i=0; i<N_pgm; i++)
		if(ULONGDATA(image)[i]>max) max=ULONGDATA(image)[i];


	tab1=(uint32_t*)calloc(max-1, sizeof(uint32_t));
	if(tab1==NULL)
	{
		fprintf(stderr, "Memory allocation error.\n");
		freeimage(image);
		freeimage(image2);
		return(1);
	}

	for(i=0; i<N_pgm; i++)
		if(ULONGDATA(image)[i]!=0) tab1[ULONGDATA(image)[i]-1]++;


	max2=0;
	for(i=0; i<N_pgm; i++)
		if(ULONGDATA(image2)[i]>max2) max2=ULONGDATA(image2)[i];


	tab2=(uint32_t*)calloc(max2-1, sizeof(uint32_t));
	if(tab2==NULL)
	{
		fprintf(stderr, "Memory allocation error.\n");
		freeimage(image);
		freeimage(image2);
		return(1);
	}

	for(i=0; i<N_pgm; i++)
		if(ULONGDATA(image2)[i]!=0) tab2[ULONGDATA(image2)[i]-1]++;


	for(i=0; i<N_pgm; i++)
	{
		if(ULONGDATA(image2)[i] != max2)
		{
			if( (ULONGDATA(image2)[i] == max2) || (ULONGDATA(image)[i] == max) ) ULONGDATA(image)[i] = max+max2;
			else if(tab2[ULONGDATA(image2)[i] -1] < tab1[ULONGDATA(image)[i] -1]) ULONGDATA(image)[i] = max+ULONGDATA(image2)[i];
		}
	}

	writeimage(image, argv[3]);

	free(tab2);
	free(tab1);
	freeimage(image);
	freeimage(image2);

	fprintf(stdout, "Please keep in mind that in both images, the highest value is considered as the separation value, and receives special processing.\n");

	return(0);
}

