
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


#define USAGE "<input_cca> <6/4 distance map> <6/4 opening map> <output_cca>"





int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *distmap, *opening, *filtermap, *t1, *birth, *result;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, l;
	list **free_faces, *border_faces;




	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=5)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}


	opening=readimage(argv[3]);
	if (opening==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
		return(1);
	}
	else if(datatype(opening)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: opening map should be a long PGM image.\n");
		return(1);
	}
	filtermap=opening;


	distmap=readimage(argv[2]);
	if (distmap==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(1);
	}
	else if(datatype(distmap)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: distance map should be a long PGM image.\n");
		return(1);
	}
	else if(rowsize(opening)!=rowsize(distmap) || colsize(opening)!=colsize(distmap) || depth(opening)!=depth(distmap))
	{
		fprintf(stderr, "Error: incompatible size between opening map and distance map.\n");
		return(1);
	}

	N=colsize(opening)*rowsize(opening)*depth(opening);
	for(i=0; i<N; i++)
	{
		ULONGDATA(filtermap)[i]=2*(ULONGDATA(opening)[i]-ULONGDATA(distmap)[i]);
	}

	freeimage(distmap);


	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CCA image supported for input\n");
		return(1);
	}
	if(rowsize(opening)!=rowsize(image)-1 || colsize(opening)!=colsize(image)-1 || depth(opening)!=depth(image)-1)
	{
		fprintf(stderr, "Error: incompatible size between input image and opening fucntion.\n");
		return(1);
	}


	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=d*ps;


	birth=allocimage(NULL, 3*rs, cs, d, VFF_TYP_4_BYTE);
	if(birth==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (1).\n");
		return(1);
	}


	result=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	if(result==NULL)
	{
		fprintf(stderr, "Error: memory allocation failed for output image.\n");
		return(1);
	}




	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	free_faces=NULL;
	cont=1;
	l=0;
	t1=NULL;
	border_faces=NULL;

	while(cont==1)
	{
		cont=0;

		free_faces=cca_collapse_directional_surfacic(image, NULL, 1, free_faces, &t1, &border_faces);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			return(1);
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

						if((l > (cca_getfacedist(image, filtermap, i, j, k, CC_AX, rs, ps))+2*ULONGDATA(birth)[3*pix]) )  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
							if((UCHARDATA(result)[pix]&CC_AX)==0)
								cca_add_complexface(result, CC_AX, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+1]==0)
							ULONGDATA(birth)[3*pix+1]=l;

						if((l > (cca_getfacedist(image, filtermap, i, j, k, CC_AY, rs, ps))+2*ULONGDATA(birth)[3*pix+1]) ) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
							if((UCHARDATA(result)[pix]&CC_AY)==0)
								cca_add_complexface(result, CC_AY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+2]==0)
							ULONGDATA(birth)[3*pix+2]=l;

						if((l > (cca_getfacedist(image, filtermap, i, j, k, CC_AZ, rs, ps))+2*ULONGDATA(birth)[3*pix+2]) ) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
							if((UCHARDATA(result)[pix]&CC_AZ)==0)
								cca_add_complexface(result, CC_AZ, pix, rs, ps);
					}

					pix++;
				}

		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
				cont=1;
		}
	}


	writeimage(result, argv[4]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(birth);
	freeimage(filtermap);
	freeimage(t1);

	list_delete(border_faces, NO_FREE_DATA);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}



