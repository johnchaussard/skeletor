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

//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<input_cc> <output_cc> (inhibit_cc)"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *inhibit, *t1;
	uint32_t i;
	list *border_faces;
	list **free_faces=NULL;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc<3 || argc>4)
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
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}


	//We read inhibit image if one was given
	inhibit=NULL;
	if(argc==4)
	{
		inhibit=readimage(argv[3]);
		if (inhibit==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
			return(-1);
		}
		else if(datatype(inhibit)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
			return(-1);
		}
	}

	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	//if(cca_collapse_method_facesandsubfaces2(image, inhibit, 0)<0)
	t1=NULL;
	border_faces=NULL;
	free_faces=cca_collapse_directional_surfacic(image, inhibit, 0, free_faces, &t1, &border_faces);
	if(free_faces==NULL)
	{
		fprintf(stderr, "Error: cca_collapse() failed.\n");
		freeimage(image);
		if(inhibit!=NULL)
			freeimage(inhibit);
		return(-1);
	}

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i], NO_FREE_DATA);
	}
	free(free_faces);



	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	if(inhibit!=NULL)
		freeimage(inhibit);
	freeimage(t1);
	list_delete(border_faces, NO_FREE_DATA);



	return(0);
}

