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

#define SURF_INSIDE 0
#define SURF_BORD 1
#define ULTIM 2

#define USAGE "<input_cc> <output_cc> <surfdetect:facet|isthmus|no> <nbtour(0 for nfnty)> <filter_map|NULL> <inhibit|NULL>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *inhibit, *distmap, *t1;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, mode, tour;
	list **free_faces, *border_faces;

	uint32_t l;

	struct xvimage *birth;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=7)
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

	birth=allocimage(NULL, 6*rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);



	if(strcmp(argv[5], "NULL")!=0)
	{
		distmap=readimage(argv[5]);
		if (distmap==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
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


	if(strcmp(argv[6], "NULL")!=0)
		inhibit=readimage(argv[6]);
	else
		inhibit=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);


	if(strcmp(argv[3], "facet")==0)
	{
		mode=SURF_BORD;
	}
	else if(strcmp(argv[3], "no")==0)
	{
		mode=ULTIM;
	}
	else if(strcmp(argv[3], "isthmus")==0)
	{
		mode=SURF_INSIDE;
	}
	else
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	tour=atoi(argv[4]);


	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	free_faces=NULL;
	cont=1;
	l=0;
	t1=NULL;
	border_faces=NULL;
	while(cont==1 && (tour==0 || l<tour))
	{
		cont=0;

		if(mode==SURF_INSIDE || mode==SURF_BORD)
			free_faces=cca_collapse_directional_surfacic(image, inhibit, 1, free_faces, &t1, &border_faces);
		else
			free_faces=cca_collapse_directional_surfacic(image, inhibit, tour, free_faces, &t1, &border_faces);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			return(-1);
		}
		l++;

		if(mode==SURF_INSIDE || mode==SURF_BORD)
		{
			pix=0;
			for(k=0; k<d; k++)
				for(j=0; j<cs; j++)
					for(i=0; i<rs; i++)
					{
						//if(l==2)
							//fprintf(stdout, "%d\n", pix);
						if(UCHARDATA(image)[pix]!=0 && ((UCHARDATA(image)[pix]&CC_VOL) == 0))
						{
							if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
							{
								if(ULONGDATA(birth)[6*pix]==0)
									ULONGDATA(birth)[6*pix]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_FXY, rs, ps)  < (l-ULONGDATA(birth)[6*pix]))
									if((UCHARDATA(inhibit)[pix]&CC_FXY)==0)
										if(mode==SURF_BORD || cca_is_face_isthmus(image, pix, CC_FXY, rs, ps))
											cca_add_complexface(inhibit, CC_FXY, pix, rs, ps);
							}

							if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
							{
								if(ULONGDATA(birth)[6*pix+1]==0)
									ULONGDATA(birth)[6*pix+1]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_FXZ, rs, ps)  < (l-ULONGDATA(birth)[6*pix+1]))
									if((UCHARDATA(inhibit)[pix]&CC_FXZ)==0)
										if(mode==SURF_BORD || cca_is_face_isthmus(image, pix, CC_FXZ, rs, ps))
											cca_add_complexface(inhibit, CC_FXZ, pix, rs, ps);
							}

							if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
							{
								if(ULONGDATA(birth)[6*pix+2]==0)
									ULONGDATA(birth)[6*pix+2]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_FYZ, rs, ps)  < (l-ULONGDATA(birth)[6*pix+2]))
									if((UCHARDATA(inhibit)[pix]&CC_FYZ)==0)
										if(mode==SURF_BORD || cca_is_face_isthmus(image, pix, CC_FYZ, rs, ps))
											cca_add_complexface(inhibit, CC_FYZ, pix, rs, ps);
							}

							if(((UCHARDATA(image)[pix]&CC_AX)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AX, rs, ps)==0 )
							{
								if(ULONGDATA(birth)[6*pix+3]==0)
									ULONGDATA(birth)[6*pix+3]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)  <= (l-ULONGDATA(birth)[6*pix+3]))
									if((UCHARDATA(inhibit)[pix]&CC_AX)==0)
										cca_add_complexface(inhibit, CC_AX, pix, rs, ps);
							}

							if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
							{
								if(ULONGDATA(birth)[6*pix+4]==0)
									ULONGDATA(birth)[6*pix+4]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)  < (l-ULONGDATA(birth)[6*pix+4]))
									if((UCHARDATA(inhibit)[pix]&CC_AY)==0)
										cca_add_complexface(inhibit, CC_AY, pix, rs, ps);
							}

							if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
							{
								if(ULONGDATA(birth)[6*pix+5]==0)
									ULONGDATA(birth)[6*pix+5]=l;

								if(distmap==NULL || cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)  < (l-ULONGDATA(birth)[6*pix+5]))
									if((UCHARDATA(inhibit)[pix]&CC_AZ)==0)
										cca_add_complexface(inhibit, CC_AZ, pix, rs, ps);
							}
						}
						pix++;
					}
		}

		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
				cont=1;
		}

		if(mode==ULTIM)
			cont=0;

		//sprintf(filename, "debug_%d.cc", l);
		//writeimage(image, filename);
	}



	writeimage(image, argv[2]);


	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhibit);
	freeimage(birth);
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


