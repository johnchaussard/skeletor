/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>


//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<edges.cc> <faces.cc> <out.cc>"

//Removes edges of edges.cc which are not contained in at least one face of faces.cc
int32_t main(int argc, char* argv[])
{
	struct xvimage *edges, *faces;
	uint32_t i, j, k, pix, rs, cs, ps, d;
	if(argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the edges image
	edges=readimage(argv[1]);
	if(edges==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(edges)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input (cc images).\n");
		return(-1);
	}

	//Then, open the faces image
	faces=readimage(argv[2]);
	if(faces==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(faces)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input (cc images).\n");
		return(-1);
	}

	rs=rowsize(edges);
	cs=colsize(edges);
	d=depth(edges);
	ps=rs*cs;

	pix=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if( (UCHARDATA(edges)[pix] & CC_AX) !=0)
					if(cca_cardinal_containers(faces, pix, i, j, k, CC_AX, rs, ps)==0)
						UCHARDATA(edges)[pix] = UCHARDATA(edges)[pix] - CC_AX;

				if( (UCHARDATA(edges)[pix] & CC_AY) !=0)
					if(cca_cardinal_containers(faces, pix, i, j, k, CC_AY, rs, ps)==0)
						UCHARDATA(edges)[pix] = UCHARDATA(edges)[pix] - CC_AY;

				if( (UCHARDATA(edges)[pix] & CC_AZ) !=0)
					if(cca_cardinal_containers(faces, pix, i, j, k, CC_AZ, rs, ps)==0)
						UCHARDATA(edges)[pix] = UCHARDATA(edges)[pix] - CC_AZ;

				UCHARDATA(edges)[pix] = UCHARDATA(edges)[pix] & (255-CC_PT);

				pix++;
			}

	cca_makecomplex(edges);


	writeimage(edges, argv[3]);

	return(0);
}



