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
#include <libeps.h>
#include <libcomplex.h>

#define USAGE "<cca_image> <eps_output_file>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	FILE *output;
	eps_scene* scene;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		freeimage(image);
		return(-1);
	}
	else if(depth(image)!=2)
	{
		fprintf(stderr, "Error: only 2d images supported\n");
		freeimage(image);
		return(-1);
	}


	//Finally, open the output file
	output = fopen(argv[2], "wb");
	if(output==NULL)
	{
		fprintf(stderr, "Error: could not create output file %s.\n", argv[2]);
		freeimage(image);
		return(-1);
	}

	scene=eps_new_scene(output, 1, rowsize(image), colsize(image));
	if(scene==NULL)
	{
		fprintf(stderr, "Error: could not create new scene.\n");
		freeimage(image);
		fclose(output);
		return(-1);
	}




	//*******************************************************
	//Conversion of image to inventor file
	//*******************************************************

	if(cca_to_eps(image, scene)!=0)
	{
		fprintf(stderr, "Error: image conversion failed.\n");
		eps_delete_scene(scene);
		fclose(output);
		freeimage(image);
		return(-1);
	}




	//****************************************************
	//Program ends
	//****************************************************
	eps_delete_scene(scene);
	freeimage(image);
	fclose(output);


	return(0);
}
