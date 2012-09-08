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
#include <liblist.h>

#define USAGE "<in.pgm> <out.cc> <faces.cc>"

int32_t main(int argc, char* argv[])
{
	struct xvimage *image, *faces;
	list *l;
	uint32_t i, rs, ps, N, k, x, y, z, nrs, nps;
	int32_t p, v;
	if(argc!=4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the inhibit image
	image=readimage(argv[1]);
	if(image==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input.\n");
		return(-1);
	}

	faces=readimage(argv[3]);
	if(faces==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[3]);
		return(-1);
	}
	if(datatype(faces)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for faces (cc images).\n");
		return(-1);
	}


	l=list_newlist(MODE_FIFO, 512);
	if(l==NULL)
	{
		fprintf(stderr, "Error: list_newlist() failed.\n");
		freeimage(image);
		freeimage(faces);
		return(-1);
	}

	rs=rowsize(image);
	ps=colsize(image)*rs;
	N=ps*depth(image);

	//Scan the image and look for curves extremities aka voxels with one neighbour
	for(i=0; i<N; i++)
	{
		//For each voxel, count the number of neighbours
		if(UCHARDATA(image)[i]!=0)
		{
			v=0;
			for(k=0; k<26; k++)
			{
				p=voisin26(i,k,rs,ps,N);
				if(p>=0 && UCHARDATA(image)[p]!=0)
					v++;
			}
			if(v<=1)
				list_push_uint32_t(l, i);
		}
	}


	if(pgm_to_cca(image)<0)
	{
		fprintf(stderr, "Error: pgm_to_cca() failed.\n");
		list_delete(l, NO_FREE_DATA);
		return(-1);
	}

	nrs=rowsize(image);
	nps=colsize(image)*nrs;
	N=nps*depth(image);
	for(i=0; i<N; i++)
		UCHARDATA(image)[i]=0;


	while(!list_isempty(l))
	{
		i=list_pop_uint32_t(l);
		x=getxfrompixnum(i, rs, ps);
		y=getyfrompixnum(i, rs, ps);
		z=getzfrompixnum(i, rs, ps);
		i=x+y*nrs+z*nps;

		if(	((UCHARDATA(faces)[i] & CC_FXY) ==0) &&
			((UCHARDATA(faces)[i] & CC_FXZ) ==0) &&
			((UCHARDATA(faces)[i] & CC_FYZ) ==0) &&
			((UCHARDATA(faces)[i+nps] & CC_FXY) ==0) &&
			((UCHARDATA(faces)[i+nrs] & CC_FXZ) ==0) &&
			((UCHARDATA(faces)[i+1] & CC_FYZ) ==0) )
			cca_add_complexface(image, CC_PT, i, nrs, nps);
	}

	writeimage(image, argv[2]);

	list_delete(l, NO_FREE_DATA);
	freeimage(image);
	freeimage(faces);

	return(0);
}


