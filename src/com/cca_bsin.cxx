/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

/*Bibliotheque perso*/
#include <libgeneral.h>

#define USAGE "<input_cca_image> <output_cca_image>"


int main()
{
	uint32_t rs, ps, x, y, z, N, cs, d, i;
	struct xvimage *image;
	list *surfaces_seg;

	if(argc!=3)
	{
		fprintf(stderr, "usage : %s %s\n", argv[0], USAGE);
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

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=d*ps;

	surface_seg=cca_simple_surface_segmentation(image);
	if(surface_seg==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation() failed.\n");
		freeimage(image);
		return(-1);
	}

	//Then, reopen the input image as it was modified...
	freeimage(image);
	image=readimage(argv[1]);





	//Scan the image and keep only the surface intersections
	i=0;
	for(z=0; z<d; z++)
		for(y=0; y<cs; y++)
			for(x=0; x<rs; x++)
			{
				if((UCHARDATA(image)[i] & CC_AX)!=0)
				{
					if(cca_cardinal_containers(image, i, x, y, z, CC_AX, rs, ps) <3)
						UCHARDATA(image)[i] = UCHARDATA(image)[i] - CC_AX;
				}
				if((UCHARDATA(image)[i] & CC_AY)!=0)
				{
					if(cca_cardinal_containers(image, i, x, y, z, CC_AY, rs, ps) <3)
						UCHARDATA(image)[i] = UCHARDATA(image)[i] - CC_AY;
				}
				if((UCHARDATA(image)[i] & CC_AZ)!=0)
				{
					if(cca_cardinal_containers(image, i, x, y, z, CC_AZ, rs, ps) <3)
						UCHARDATA(image)[i] = UCHARDATA(image)[i] - CC_AZ;
				}
				i++;
			}

	//Remove faces and volumes
	for(i=0; i<N; i++)
		UCHARDATA(image)[i]&= (CC_AX | CC_AY | CC_AZ | CC_PT);

	writeimage(image, argv[2]);

	freeimage(image);

	return(0);
}


