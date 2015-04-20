
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


#define USAGE "<input_pgm> <inhibit_cc|NULL> <output_pgm> (<output_curvilinear_elements_pgm>)"


uint32_t cca_highest_value(struct xvimage *cca_image, struct xvimage *distmap, uint32_t i, uint32_t j, uint32_t k, unsigned char face_type, uint32_t rs, uint32_t ps, struct xvimage* output)
{
	uint32_t pix, bord, newps, newrs, max;

	bord=cca_is_face_onborder(cca_image, face_type, i, j, k);

	newps = (rowsize(cca_image)-1)*(colsize(cca_image)-1);
	newrs=rowsize(cca_image)-1;

	pix=i+j*(newrs)+k*(newps);

	if(UCHARDATA(output)[pix]==255)
		return pix;

	switch(face_type)
	{
		case(CC_FXY):
			if( (bord & Z_BORDER_MIN) == 0 )
			{
				if (UCHARDATA(output)[pix-newps]==255)
					return(pix-newps);
			}
		break;

		case(CC_FYZ):
			if( (bord & X_BORDER_MIN) == 0 )
			{
				if (UCHARDATA(output)[pix-1]==255)
					return(pix-1);
			}
		break;

		case(CC_FXZ):
			if( (bord & Y_BORDER_MIN) == 0 )
			{
				if (UCHARDATA(output)[pix-newrs]==255)
					return(pix-newrs);
			}
		break;

		case(CC_AX):
			if( ((bord & Y_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newrs]==255) ) return(pix-newrs);
			if( ((bord & Z_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newps]==255) ) return(pix-newps);
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newrs-newps]==255) ) return(pix-newrs-newps);
		break;

		case(CC_AY):
			if( ((bord & X_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-1]==255) ) return(pix-1);
			if( ((bord & Z_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newps]==255) ) return(pix-newps);
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-1-newps]==255) ) return(pix-1-newps);
		break;

		case(CC_AZ):
			if( ((bord & Y_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newrs]==255) ) return(pix-newrs);
			if( ((bord & X_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-1]==255) ) return(pix-1);
			if( ((bord & X_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (UCHARDATA(output)[pix-newrs-1]==255) ) return(pix-newrs-1);
		break;

		default:
			fprintf(stderr, "cca_getfacedist(): Wrong face type given.\n");
	}


	switch(face_type)
	{
		case(CC_FXY):
			if( (bord & Z_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-newps])
					return(pix);
				else if (ULONGDATA(distmap)[pix]<ULONGDATA(distmap)[pix-newps])
					return(pix-newps);
				else
					return(pix);
			}
			else
				return(pix);
		break;

		case(CC_FYZ):
			if( (bord & X_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-1])
					return(pix);
				else if (ULONGDATA(distmap)[pix]<ULONGDATA(distmap)[pix-1])
					return(pix-1);
				else
					return(pix);
			}
			else
				return(pix);
		break;

		case(CC_FXZ):
			if( (bord & Y_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-newrs])
					return(pix);
				else if (ULONGDATA(distmap)[pix]<ULONGDATA(distmap)[pix-newrs])
					return(pix-newrs);
				else
					return(pix);
			}
			else
				return (pix);
		break;

		case(CC_AX):
			max=pix;
			if( ((bord & Y_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newrs]) ) max=pix-newrs;
			if( ((bord & Z_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newps]) ) max=pix-newps;
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newrs-newps]) ) max=pix-newrs-newps;

			return(max);

		break;

		case(CC_AY):
			max=pix;
			if( ((bord & X_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-1]) ) max=pix-1;
			if( ((bord & Z_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newps]) ) max=pix-newps;
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-1-newps]) ) max=pix-1-newps;

			return(max);

		break;

		case(CC_AZ):
			max=pix;
			if( ((bord & Y_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newrs]) ) max=pix-newrs;
			if( ((bord & X_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-1]) ) max=pix-1;
			if( ((bord & X_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[pix-newrs-1]) ) max=pix-newrs-1;

			return(max);

		break;

		default:
			fprintf(stderr, "cca_getfacedist(): Wrong face type given.\n");
	}

	return(0);
}


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
		memcpy(distmap_in->image_data, t1->image_data, N*4);
		freeimage(t1);
	}
	else
		freeimage(t2);

	return(0);
}



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *filtermap_curv, *filtermap_surf, *t1, *inhib, *birth, *pgm_map, *output, *output_curv;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, l, pgm_cpt, t, N2;
	list **free_faces;




	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=4 && argc!=5)
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
		freeimage(image);
		return(-1);
	}

	d=depth(image);
	if(d==1)
	{
		fprintf(stderr, "Error: only 3d images supported for surfacic skeletonisation (use cca_curvskel for 2d images)\n");
		freeimage(image);
		return(-1);
	}

	N=rowsize(image)*colsize(image)*d;

	pgm_map=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	pgm_cpt=1;

	output=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	if(argc==5)
		output_curv=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	else
		output_curv=output;


	//***********************************************
	//Computing filter map
	//***********************************************
	filtermap_surf=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(filtermap_surf==NULL)
	{
		fprintf(stderr, "Error: memory allocation failed for filter map.\n");
		freeimage(image);
		return(-1);
	}

	//Inverse image
	for (i = 0; i < N; i++) // inverse l'image
		UCHARDATA(image)[i] = 255 - UCHARDATA(image)[i];

	if (! ldist(image, 6, filtermap_surf))
	{
		fprintf(stderr, "Error: ldist failed.\n");
		freeimage(filtermap_surf);
		freeimage(image);
		return(-1);
	}

	filtermap_curv=copyimage(filtermap_surf);
	if(filtermap_curv==NULL)
	{
		fprintf(stderr, "Error: copyimage failed.\n");
		freeimage(filtermap_surf);
		freeimage(image);
		return(-1);
	}

	if(distance_dilat(filtermap_surf) != 0)
	{
		fprintf(stderr, "Error: distance dilat failed.\n");
		freeimage(filtermap_curv);
		freeimage(filtermap_surf);
		freeimage(image);
		return(-1);
	}

	for(i=0; i<N; i++)
	{
		ULONGDATA(filtermap_surf)[i]=ULONGDATA(filtermap_surf)[i]-ULONGDATA(filtermap_curv)[i];
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
		freeimage(filtermap_curv);
		freeimage(filtermap_surf);
		return(-1);
	}

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=d*ps;


	birth=allocimage(NULL, 6*rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	if(birth==NULL)
	{
		fprintf(stderr, "Error, Memory allocation error (1).\n");
		freeimage(filtermap_curv);
		freeimage(filtermap_surf);
		freeimage(image);
		return(-1);
	}


	if(strcmp(argv[2], "NULL")==0)
	{
		inhib=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
		if(inhib==NULL)
		{
			fprintf(stderr, "Error: memory allocation failed for inhibit image.\n");
			freeimage(filtermap_curv);
			freeimage(filtermap_surf);
			freeimage(image);
			return(-1);
		}
	}
	else
	{
		inhib=readimage(argv[2]);
		if(inhib==NULL)
		{
			fprintf(stderr, "Error: inhibit image could not be found.\n");
			freeimage(filtermap_curv);
			freeimage(filtermap_surf);
			freeimage(image);
			return(-1);
		}
		else if(rowsize(image)!=rowsize(inhib) || colsize(image)!=colsize(inhib) || depth(image)!=depth(inhib) || datatype(inhib)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error: inhibit image has bad type or bad size.\n");
			freeimage(filtermap_curv);
			freeimage(filtermap_surf);
			freeimage(image);
			return(-1);
		}
	}



	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	free_faces=NULL;
	cont=1;
	l=0;
	t1=NULL;




	while(cont==1)
	{
		cont=0;

		free_faces=cca_collapse_directional_surfacic_for_pgm_purpose(image, inhib, 1, free_faces, &t1, pgm_map, &pgm_cpt);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			freeimage(filtermap_curv);
			freeimage(filtermap_surf);
			freeimage(image);
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
						if(ULONGDATA(birth)[6*pix]==0)
							ULONGDATA(birth)[6*pix]=l;

						//Paramètre de filtrage ajustable, scal invariant...
						if(l > ((cca_getfacedist(image, filtermap_surf, i, j, k, CC_AX, rs, ps) + 2*ULONGDATA(birth)[6*pix]) *1))  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
							if((UCHARDATA(inhib)[pix]&CC_AX)==0)
								cca_add_complexface(inhib, CC_AX, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[6*pix+1]==0)
							ULONGDATA(birth)[6*pix+1]=l;

						if(l > ((cca_getfacedist(image, filtermap_surf, i, j, k, CC_AY, rs, ps) + 2*ULONGDATA(birth)[6*pix+1]) *1)) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
							if((UCHARDATA(inhib)[pix]&CC_AY)==0)
								cca_add_complexface(inhib, CC_AY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[6*pix+2]==0)
							ULONGDATA(birth)[6*pix+2]=l;

						if(l > ((cca_getfacedist(image, filtermap_surf, i, j, k, CC_AZ, rs, ps) + 2*ULONGDATA(birth)[6*pix+2]) *1)) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
							if((UCHARDATA(inhib)[pix]&CC_AZ)==0)
								cca_add_complexface(inhib, CC_AZ, pix, rs, ps);
					}

					//Modif John : here, we can choose to add BIRTH to the values or not... It will remove more or less surfaces.
					if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+3]==0)
							ULONGDATA(birth)[6*pix+3]=l;

						if(( (l-ULONGDATA(birth)[6*pix+3]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXY, rs, ps)+ULONGDATA(birth)[6*pix+3]) )
							if((UCHARDATA(inhib)[pix]&CC_FXY)==0)
								cca_add_complexface(inhib, CC_FXY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+4]==0)
							ULONGDATA(birth)[6*pix+4]=l;

						if(( (l-ULONGDATA(birth)[6*pix+4]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXZ, rs, ps)+ULONGDATA(birth)[6*pix+4]) )
							if((UCHARDATA(inhib)[pix]&CC_FXZ)==0)
								cca_add_complexface(inhib, CC_FXZ, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+5]==0)
							ULONGDATA(birth)[6*pix+5]=l;

						if(( (l-ULONGDATA(birth)[6*pix+5]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FYZ, rs, ps)+ULONGDATA(birth)[6*pix+5]) )
							if((UCHARDATA(inhib)[pix]&CC_FYZ)==0)
								cca_add_complexface(inhib, CC_FYZ, pix, rs, ps);
					}


					pix++;
				}

		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
				cont=1;
		}

	}



	cca_keep_only_cells(image);


	pix=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if((UCHARDATA(image)[pix]&CC_AX)!=0)
				{
					UCHARDATA(output_curv)[cca_highest_value(image, pgm_map, i, j, k, CC_AX, rs, ps, output_curv)]=255;
				}

				if((UCHARDATA(image)[pix]&CC_AY)!=0)
				{
					UCHARDATA(output_curv)[cca_highest_value(image, pgm_map, i, j, k, CC_AY, rs, ps, output_curv)]=255;
				}

				if((UCHARDATA(image)[pix]&CC_AZ)!=0)
				{
					UCHARDATA(output_curv)[cca_highest_value(image, pgm_map, i, j, k, CC_AZ, rs, ps, output_curv)]=255;
				}

				if((UCHARDATA(image)[pix]&CC_FXY)!=0)
				{
					UCHARDATA(output)[cca_highest_value(image, pgm_map, i, j, k, CC_FXY, rs, ps, output)]=255;
				}

				if((UCHARDATA(image)[pix]&CC_FXZ)!=0)
				{
					UCHARDATA(output)[cca_highest_value(image, pgm_map, i, j, k, CC_FXZ, rs, ps, output)]=255;
				}

				if((UCHARDATA(image)[pix]&CC_FYZ)!=0)
				{
					UCHARDATA(output)[cca_highest_value(image, pgm_map, i, j, k, CC_FYZ, rs, ps, output)]=255;
				}

				pix++;
			}


	writeimage(output, argv[3]);
	if(argc==5)
		writeimage(output_curv, argv[4]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhib);
	freeimage(birth);
	freeimage(filtermap_curv);
	freeimage(filtermap_surf);
	freeimage(t1);
	freeimage(output);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}





