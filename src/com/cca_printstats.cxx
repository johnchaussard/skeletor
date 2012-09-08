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

#define USAGE "<input_cc>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	int32_t N, volume, face, edge, vertice, i, ps, rs, beurk, isolated;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=2)
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


	//*******************************************************
	//Only keep cells and then, count the elements...
	//*******************************************************


	N=rowsize(image)*colsize(image)*depth(image);
	rs=rowsize(image); ps=colsize(image)*rowsize(image);
	volume=0;face=0;edge=0;vertice=0;isolated=0;beurk=0;
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i]&CC_VOL) != 0)
			volume++;

		if( (UCHARDATA(image)[i]&CC_FXY) != 0)
			face++;

		if( (UCHARDATA(image)[i]&CC_FXZ) != 0)
			face++;

		if( (UCHARDATA(image)[i]&CC_FYZ) != 0)
			face++;

		if( (UCHARDATA(image)[i]&CC_AX) != 0)
			edge++;

		if( (UCHARDATA(image)[i]&CC_AY) != 0)
			edge++;

		if( (UCHARDATA(image)[i]&CC_AZ) != 0)
			edge++;

		if( (UCHARDATA(image)[i]&CC_PT) != 0)
		{
			vertice++;

			if( cca_cardinal_containers(image, i, i%rs, (i%ps)/rs, i/ps, CC_PT, rs, ps)==0)
				isolated++;
		}

		if( cca_cardinal_containers(image, i, i%rs, (i%ps)/rs, i/ps, CC_AX, rs, ps)==4)
			beurk++;

		if( cca_cardinal_containers(image, i, i%rs, (i%ps)/rs, i/ps, CC_AY, rs, ps)==4)
			beurk++;

		if( cca_cardinal_containers(image, i, i%rs, (i%ps)/rs, i/ps, CC_AZ, rs, ps)==4)
			beurk++;


	}

	fprintf(stdout, "The cubical complex is composed of:\n\t%d volume(s),\n\t%d face(s),\n\t%d edge(s),\n\t%d vertice(vertex) (%d isolated)\nEuler: %d\nFour cardinal vertices: %d \n", volume, face, edge, vertice, isolated, vertice-edge+face-volume, beurk);


	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);


	return(0);
}

