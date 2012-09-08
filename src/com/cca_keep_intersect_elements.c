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

//Bibliotheque perso
#include <libinventor.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<cca_input> <cca_output>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *input, *output;
	uint32_t i, rs_cca, cs_cca, d_cca, N_cca, ps_cca;
	int32_t face;
	unsigned char border_code;


	//*******************************************
	//Open skeleton image
	//*******************************************
	if (argc!=3)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	input=readimage(argv[1]);
	if(input==NULL)
	{
		fprintf(stderr, "Could not open image %s\n", argv[1]);
		return(1);
	}

	rs_cca=rowsize(input);
	cs_cca=colsize(input);
	d_cca=depth(input);
	ps_cca=rs_cca*cs_cca;
	N_cca=ps_cca*d_cca;



	output=allocimage(NULL, rs_cca, cs_cca, d_cca, VFF_TYP_1_BYTE);

	cca_keep_only_cells(input);


	for(i=0; i<N_cca; i++)
	{
		for(face=CC_VOL/2; face>0; face=face/2)
		{
			border_code=cca_is_face_onborder_v2(face, i, rs_cca, cs_cca, d_cca);
			if(cca_face_is_an_intersection_face(input, face, i, rs_cca, ps_cca, border_code))
				cca_add_complexface(output, face, i, rs_cca, ps_cca);
		}
	}




	writeimage(output, argv[2]);

	freeimage(output);
	freeimage(input);



	return(0);
}


