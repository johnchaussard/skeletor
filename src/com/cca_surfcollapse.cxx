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


#define USAGE "<input_cc> <output_cc> <filter_map|NULL>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *distmap, *result, *t1, *birth;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont;
	list **free_faces, *border_faces;
	uint32_t l;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=4)
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

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;
	N=ps*d;

	result=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	if(result==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (2).\n");
		return(-1);
	}

	birth=allocimage(NULL, 3*rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(birth==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (1).\n");
		return(-1);
	}

	if(strcmp(argv[3], "NULL")!=0)
	{
		distmap=readimage(argv[3]);
		if (distmap==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
			return(-1);
		}
		else if(datatype(distmap)!=VFF_TYP_4_BYTE)
		{
			fprintf(stderr, "Error: only 4 byte per pixel CC image supported\n");
			return(-1);
		}
	}
	else
		distmap=NULL;



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

		free_faces=cca_collapse_directional_surfacic(image, result, 1, free_faces, &t1, &border_faces);
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
					if((UCHARDATA(image)[pix]&CC_VOL)==0)
					{
						if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
						{
							if(ULONGDATA(birth)[3*pix]==0)
								ULONGDATA(birth)[3*pix]=l;

							if(distmap==NULL || ( (l-ULONGDATA(birth)[3*pix]) > cca_getfacedist(image, distmap, i, j, k, CC_FXY, rs, ps)) )
								if((UCHARDATA(result)[pix]&CC_FXY)==0)
									cca_add_complexface(result, CC_FXY, pix, rs, ps);
						}

						if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
						{
							if(ULONGDATA(birth)[3*pix+1]==0)
								ULONGDATA(birth)[3*pix+1]=l;

							if(distmap==NULL || ( (l-ULONGDATA(birth)[3*pix+1]) > cca_getfacedist(image, distmap, i, j, k, CC_FXZ, rs, ps)) )
								if((UCHARDATA(result)[pix]&CC_FXZ)==0)
									cca_add_complexface(result, CC_FXZ, pix, rs, ps);
						}

						if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
						{
							if(ULONGDATA(birth)[3*pix+2]==0)
								ULONGDATA(birth)[3*pix+2]=l;

							if(distmap==NULL || ( (l-ULONGDATA(birth)[3*pix+2]) > cca_getfacedist(image, distmap, i, j, k, CC_FYZ, rs, ps)) )
								if((UCHARDATA(result)[pix]&CC_FYZ)==0)
									cca_add_complexface(result, CC_FYZ, pix, rs, ps);
						}
					}
					pix++;
				}



		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
				cont=1;
		}



	}


	writeimage(result, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(result);
	if(distmap!=NULL)
		freeimage(distmap);
	freeimage(t1);
	list_delete(border_faces, NO_FREE_DATA);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}



