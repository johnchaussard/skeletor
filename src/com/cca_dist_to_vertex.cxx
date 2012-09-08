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


//coeff should be inclusive for keeping
#define USAGE "<cca_input_image> <pgm_input_mask> <pgm_border_mask> <distc6_image> <pgm_output_image>"




int32_t main(int argc, char *argv[])
{
	struct xvimage *cca_image, *mask, *result, *border, *distc6;



	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=6)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//Read the input image
	cca_image=readimage(argv[1]);
	if (cca_image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(cca_image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: image %s should be CC image\n", argv[1]);
		return(1);
	}

	//Read the mask image
	mask=readimage(argv[2]);
	if (mask==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(1);
	}
	else if(datatype(mask)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: image %s should be char image\n", argv[2]);
		return(1);
	}


	border=readimage(argv[3]);
	if (border==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
		return(1);
	}
	else if(datatype(border)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: image %s should be char image\n", argv[3]);
		return(1);
	}


	distc6=readimage(argv[4]);
	if(distc6==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[4]);
		return(1);
	}
	else if(datatype(distc6)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: image %s should be long image\n", argv[4]);
		return(1);
	}


	result = compute_closest_cca_vertex_to_pgm(cca_image, mask, border, distc6);
	if(result==NULL)
	{
		fprintf(stderr, "Error: subfunction failed.\n");
		return(1);
	}


	fprintf(stdout, "bibi %d\n", ULONGDATA(result)[129+85*rowsize(result)+433*rowsize(result)*colsize(result)]);

	writeimage(result, argv[5]);

	freeimage(cca_image);
	freeimage(mask);
	freeimage(border);
	freeimage(result);

	return(0);
}
