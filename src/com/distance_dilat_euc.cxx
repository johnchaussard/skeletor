/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>
#include <libgeneral.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

#define USAGE "<input_euc_distmap> <mask> <output_opening>"

typedef struct mapair //y'en a pas deux...
{
	float value;
	uint32_t pixnumber;
} mapair;


int32_t qsort_function_on_mapair(const void* inta, const void* intb)
{
	if(((mapair*)(inta))->value < ((mapair*)(intb))->value)
		return (-1);
	else if(((mapair*)(inta))->value > ((mapair*)(intb))->value)
		return(1);
	else
		return(0);
}


int main(int argc, char* argv[])
{
	struct xvimage *distmap_in, *mask;
	uint32_t i, rs, cs, d, ps, N, pix, x, y, z, p, cx, cy, cz, minx, miny, minz, maxx, maxy, maxz;
	int32_t si;
	float r;
	mapair *tab_pair;



	if(argc!=4)
	{
		fprintf(stdout, "%s\n", USAGE);
		return(-1);
	}

	distmap_in=readimage(argv[1]);
	if(distmap_in==NULL)
	{
		fprintf(stderr, "Could not read image %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(distmap_in)!=VFF_TYP_FLOAT)
	{
		fprintf(stderr, "Distance map must be a long image.\n");
		return(-1);
	}

	mask=readimage(argv[2]);

	rs=rowsize(distmap_in);
	cs=colsize(distmap_in);
	d=depth(distmap_in);
	ps=rs*cs;
	N=ps*d;
	pix=0;

	for(i=0; i<N; i++)
	{
		if(FLOATDATA(distmap_in)[i]>0.0)// && UCHARDATA(mask)[i]>0)
			pix++;
	}


	tab_pair=(mapair*)calloc(pix, sizeof(mapair));


	pix=0;
	for(i=0; i<N; i++)
	{
		if(FLOATDATA(distmap_in)[i]!=0)// && UCHARDATA(mask)[i]!=0)
		{
			tab_pair[pix].value=FLOATDATA(distmap_in)[i];
			tab_pair[pix].pixnumber=i;
			pix++;
		}
	}

	qsort(tab_pair, pix, sizeof(mapair), qsort_function_on_mapair);




	for(si=pix-1; si>=0; si--)
	{
		r=tab_pair[si].value;
		cx=getxfrompixnum(tab_pair[si].pixnumber, rs, ps);
		maxx=cx+(uint32_t)r+1;
		minx=cx-(uint32_t)r-1;

		cy=getyfrompixnum(tab_pair[si].pixnumber, rs, ps);
		maxy=cy+(uint32_t)r+1;
		miny=cy-(uint32_t)r-1;

		cz=getzfrompixnum(tab_pair[si].pixnumber, rs, ps);
		maxz=cz+(uint32_t)r+1;
		minz=cz-(uint32_t)r-1;

		if(minx < 0)minx=0;
		if(miny < 0)miny=0;
		if(minz < 0)minz=0;

		if(maxx >= rs) maxx=rs-1;
		if(maxy >= cs) maxy=cs-1;
		if(maxz >= d) maxz=d-1;

		for(z=minz; z<=maxz; z++)
		for(y=miny; y<=maxy; y++)
		for(x=minx; x<=maxx; x++)
		{
			if(((cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z)) <= r*r)
			{
				p=x+y*rs+z*ps;
				if(FLOATDATA(distmap_in)[p]<=r) FLOATDATA(distmap_in)[p]=r;
			}
		}

		fprintf(stdout, "%d\n", si);
	}

	writeimage(distmap_in, argv[3]);

	return(0);
}

