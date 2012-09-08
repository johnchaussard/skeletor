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

#define USAGE "<in.cc> x y z w h d <out.cc>"


int32_t main(int argc, char* argv[])
{
	struct xvimage *imagein, *imageout, *imageand;
	uint32_t i, rs, ps, d, N, x, y, z, w, h, de, supx, supy, supz, j, k, nrs, nps, nde, ncs, cs, nN;
	if(argc!=9)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	//First, open the inhibit image
	imagein=readimage(argv[1]);
	if(imagein==NULL)
	{
		fprintf(stderr, "Error, could not open %s\n", argv[1]);
		return(-1);
	}
	if(datatype(imagein)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error, only one byte image supported for input (cc images).\n");
		return(-1);
	}

	rs=rowsize(imagein);
	cs=colsize(imagein);
	ps=rs*cs;
	d=depth(imagein);
	N=ps*d;

	x=atoi(argv[2]);
	y=atoi(argv[3]);
	z=atoi(argv[4]);
	w=atoi(argv[5]);
	h=atoi(argv[6]);
	de=atoi(argv[7]);

	imageout=allocimage(NULL, w+1, h+1, de+1, VFF_TYP_1_BYTE);
	imageand=allocimage(NULL, w+1, h+1, de+1, VFF_TYP_1_BYTE);

	nrs=w+1;
	ncs=h+1;
	nde=de+1;
	nps=nrs*ncs;
	nN=nps*nde;

	supx=x+w;supy=y+h;supz=z+de;
	if(supx>=rs) supx=rs;
	if(supy>=cs) supy=cs;
	if(supz>=d) supz=d;

	for(k=z; k<supz; k++)
		for(j=y; j<supy; j++)
			for(i=x; i<supx; i++)
			{
				UCHARDATA(imageout)[(i-x)+(j-y)*nrs+(k-z)*nps]=UCHARDATA(imagein)[i+j*rs+k*ps];
			}

	freeimage(imagein);

	for(i=0; i<w; i++)
		for(j=0; j<h; j++)
			for(k=0; k<de; k++)
			{
				cca_add_complexface(imageand, CC_VOL, i+j*nrs+k*nps, nrs, nps);
			}

	for(i=0; i<nN; i++)
		UCHARDATA(imageout)[i]&=UCHARDATA(imageand)[i];

	writeimage(imageout, argv[8]);

	freeimage(imageout);
	freeimage(imageand);

	return(0);
}

