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


#define USAGE "<input_cc> <output_khal>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *result, *t1;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, k_ps, k_rs;
	list **free_faces, *border_faces;


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
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;
	N=ps*d;

	//Result is khalimsky image
	result=allocimage(NULL, 2*rowsize(image) -1, 2*colsize(image) -1, 2*depth(image) -1, VFF_TYP_4_BYTE);
	if(result==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (2).\n");
		return(-1);
	}

	k_rs=rowsize(result);
	k_ps=colsize(result)*k_rs;



	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	free_faces=NULL;
	cont=1;
	t1=NULL;
	border_faces=NULL;

	while(cont==1)
	{
		cont=0;

		free_faces=cca_collapse_directional_surfacic(image, NULL, 1, free_faces, &t1, &border_faces);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			return(-1);
		}

		pix=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if((UCHARDATA(image)[pix]&CC_VOL)==0)
					{
						if(((UCHARDATA(image)[pix]&CC_AX)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AX, rs, ps)==0 )
							ULONGDATA(result)[(2*k)*k_ps + (2*j)*k_rs + (2*i+1)]++;

						if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
							ULONGDATA(result)[(2*k)*k_ps + (2*j+1)*k_rs + (2*i)]++;

						if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
							ULONGDATA(result)[(2*k+1)*k_ps + (2*j)*k_rs + (2*i)]++;

						if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
							ULONGDATA(result)[(2*k)*k_ps + (2*j+1)*k_rs + (2*i+1)]++;

						if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
							ULONGDATA(result)[(2*k+1)*k_ps + (2*j)*k_rs + (2*i+1)]++;

						if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
							ULONGDATA(result)[(2*k+1)*k_ps + (2*j+1)*k_rs + (2*i)]++;
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
	freeimage(t1);

	list_delete(border_faces, NO_FREE_DATA);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}



