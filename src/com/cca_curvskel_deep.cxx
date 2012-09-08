
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
#include <mcgeo.h>
#include <ldist.h>

//Bibliotheque perso
#include <libcomplex.h>


#define USAGE "<input_pgm> <output_cc>"


int32_t distance_dilat(struct xvimage *distmap_in)
{
	uint32_t N, rs, cs, ps, d, i, j, k, pix;
	unsigned long max, t;
	struct xvimage *t1, *t2, *temp;

	rs=rowsize(distmap_in);
	cs=colsize(distmap_in);
	d=depth(distmap_in);
	ps=rs*cs;
	N=rs*cs*d;

	if(datatype(distmap_in)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "distance_dilat(): Distance map must be a long image.\n");
		return(-1);
	}

	//Allocate memory for output image
	t2=allocimage(NULL, rs, cs, d, VFF_TYP_4_BYTE);
	if(t2==NULL)
	{
		fprintf(stderr, "distance_dilat(): Memory allocation error.\n");
		return(-1);
	}

	t1=distmap_in;

	max=ULONGDATA(distmap_in)[0];
	for(i=1; i<N; i++)
	{
		if(ULONGDATA(distmap_in)[i]>max)
			max=ULONGDATA(distmap_in)[i];
	}

	t=0;
	while(t<max)
	{
		pix=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if(ULONGDATA(t1)[pix]>=t)
					{
						if(ULONGDATA(t1)[pix]>ULONGDATA(t2)[pix])
							ULONGDATA(t2)[pix]=ULONGDATA(t1)[pix];

						if(i>0 && (ULONGDATA(t1)[pix-1]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix-1]=ULONGDATA(t1)[pix];

						if(i<rs-1 && (ULONGDATA(t1)[pix+1]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix+1]=ULONGDATA(t1)[pix];

						if(j>0 && (ULONGDATA(t1)[pix-rs]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix-rs]=ULONGDATA(t1)[pix];

						if(j<cs-1 && (ULONGDATA(t1)[pix+rs]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix+rs]=ULONGDATA(t1)[pix];

						if(k>0 && (ULONGDATA(t1)[pix-ps]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix-ps]=ULONGDATA(t1)[pix];

						if(k<d-1 && (ULONGDATA(t1)[pix+ps]<ULONGDATA(t1)[pix]))
							ULONGDATA(t2)[pix+ps]=ULONGDATA(t1)[pix];
					}

					pix++;
				}
		t++;
		temp=t1;
		t1=t2;
		t2=temp;
	}

	if(max%2 == 1)
	{
		memcpy(distmap_in->image_data, t1->image_data, N*sizeof(unsigned long));
		freeimage(t1);
	}
	else
		freeimage(t2);

	return(0);
}



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *filtermap, *distmap, *t1, *t2, *inhib, *birth;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, l;
	list **free_faces;




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
		fprintf(stderr, "Error: only 1 byte per pixel PGM image supported\n");
		return(-1);
	}

	d=depth(image);
	N=rowsize(image)*colsize(image)*d;



	//***********************************************
	//Computing filter map
	//***********************************************
	filtermap=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(filtermap==NULL)
	{
		fprintf(stderr, "Error: memory allocation failed for filter map.\n");
		freeimage(image);
		return(-1);
	}

	//Inverse image
	for (i = 0; i < N; i++) // inverse l'image
		UCHARDATA(image)[i] = 255 - UCHARDATA(image)[i];

	if(d==1)
	{
		if (! ldist(image, 4, filtermap))
		{
			fprintf(stderr, "Error: ldist failed.\n");
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}

		distmap=copyimage(filtermap);
		if(distmap==NULL)
		{
			fprintf(stderr, "Error: copyimage failed.\n");
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}

		if(distance_dilat(filtermap) != 0)
		{
			fprintf(stderr, "Error: distance dilat failed.\n");
			freeimage(filtermap);
			freeimage(distmap);
			freeimage(image);
			return(-1);
		}

		for(i=0; i<N; i++)
			ULONGDATA(filtermap)[i]=ULONGDATA(filtermap)[i]-ULONGDATA(distmap)[i];

		freeimage(distmap);
	}
	else
	{
		if (! ldist(image, 6, filtermap))
		{
			fprintf(stderr, "Error: ldist failed.\n");
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}


		if(distance_dilat(filtermap) != 0)
		{
			fprintf(stderr, "Error: distance dilat failed.\n");
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}

	}


	//Inverse image
	for (i = 0; i < N; i++) // inverse l'image
		UCHARDATA(image)[i] = 255 - UCHARDATA(image)[i];




	//********************************************
	//Prepare input image
	//********************************************

	if(pgm_to_cca(image)<0)
	{
		fprintf(stderr, "Error: pgm_to_cca() failed.\n");
		freeimage(image);
		freeimage(filtermap);
		return(-1);
	}

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=d*ps;


	birth=allocimage(NULL, 3*rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(birth==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (1).\n");
		freeimage(filtermap);
		freeimage(image);
		return(-1);
	}


	inhib=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	if(inhib==NULL)
	{
		fprintf(stderr, "Error: memory allocation failed for inhibit image.\n");
		freeimage(filtermap);
		freeimage(image);
		return(-1);
	}




	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	free_faces=NULL;
	cont=1;
	l=0;
	t1=NULL;
	t2=NULL;

	while(cont==1)
	{
		cont=0;

		//free_faces=cca_collapse_directional_surfacic(image, inhib, 1, free_faces, &t1);
		free_faces=cca_collapse_directional_surfacic_enhanced(image, inhib, 1, free_faces, &t1, &t2);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			return(-1);
		}
		l++;

		pix=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if(((UCHARDATA(image)[pix]&CC_AX)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AX, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix]==0)
							ULONGDATA(birth)[3*pix]=l;

						if(( (l-ULONGDATA(birth)[3*pix]) > (cca_getfacedist(image, filtermap, i, j, k, CC_AX, rs, ps))) )  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
							if((UCHARDATA(inhib)[pix]&CC_AX)==0)
								cca_add_complexface(inhib, CC_AX, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+1]==0)
							ULONGDATA(birth)[3*pix+1]=l;

						if(( (l-ULONGDATA(birth)[3*pix+1]) > (cca_getfacedist(image, filtermap, i, j, k, CC_AY, rs, ps))) ) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
							if((UCHARDATA(inhib)[pix]&CC_AY)==0)
								cca_add_complexface(inhib, CC_AY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+2]==0)
							ULONGDATA(birth)[3*pix+2]=l;

						if(( (l-ULONGDATA(birth)[3*pix+2]) > (cca_getfacedist(image, filtermap, i, j, k, CC_AZ, rs, ps))) ) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
							if((UCHARDATA(inhib)[pix]&CC_AZ)==0)
								cca_add_complexface(inhib, CC_AZ, pix, rs, ps);
					}

					pix++;
				}

		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
				cont=1;
		}
	}


	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhib);
	freeimage(birth);
	freeimage(filtermap);
	freeimage(t1);
	freeimage(t2);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}



