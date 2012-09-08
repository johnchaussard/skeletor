
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


#define USAGE "<input_cc> <output_cc> (<inhibit_cc>)"




int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *filtermap_curv, *filtermap_surf, *t1, *inhib, *birth;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, l, faces;
	list **free_faces;
	uint32_t** faces_to_neighbours;
	uint32_t* faces_position;




	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3 && argc!=4)
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
		fprintf(stderr, "Error: only CC image supported as input\n");
		freeimage(image);
		return(-1);
	}

	d=depth(image);
	if(d==1)
	{
		fprintf(stderr, "Error: only 3d images supported\n");
		freeimage(image);
		return(-1);
	}

	N=rowsize(image)*colsize(image)*d;

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=d*ps;


	//***********************************************
	//Computing filter map
	//***********************************************
	//First of all, count how many faces in the object
	faces=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
				{
					faces++;
				}

				if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
				{
					faces++;
				}

				if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
				{
					faces++;
				}
			}

	faces_position=calloc(faces, sizeof(uint32_t));

	faces_to_neighbours=calloc(faces, sizeof(uint32_t*));
	for(i=0; i<faces; i++)
	{
		faces_to_neighbour[i]=calloc(12, sizeof(uint32_t));
	}

	faces=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
				{
					faces_position[faces]=pix*3;
					faces++;
				}

				if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
				{
					faces_position[faces]=pix*3+1;
					faces++;
				}

				if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
				{
					faces_position[faces]=pix*3+2;
					faces++;
				}
			}




	//First, compute a D1 distance map on the surfaces
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(((UCHARDATA(image)[pix]&CC_AX)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AX, rs, ps)==0 )
				{
					if(ULONGDATA(birth)[6*pix]==0)
						ULONGDATA(birth)[6*pix]=l;

					if(( (l-ULONGDATA(birth)[6*pix]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AX, rs, ps))) )  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
						if((UCHARDATA(inhib)[pix]&CC_AX)==0)
							cca_add_complexface(inhib, CC_AX, pix, rs, ps);
				}

				if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
				{
					if(ULONGDATA(birth)[6*pix+1]==0)
						ULONGDATA(birth)[6*pix+1]=l;

					if(( (l-ULONGDATA(birth)[6*pix+1]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AY, rs, ps))) ) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
						if((UCHARDATA(inhib)[pix]&CC_AY)==0)
							cca_add_complexface(inhib, CC_AY, pix, rs, ps);
				}

				if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
				{
					if(ULONGDATA(birth)[6*pix+2]==0)
						ULONGDATA(birth)[6*pix+2]=l;

					if(( (l-ULONGDATA(birth)[6*pix+2]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AZ, rs, ps))) ) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
						if((UCHARDATA(inhib)[pix]&CC_AZ)==0)
							cca_add_complexface(inhib, CC_AZ, pix, rs, ps);
				}

					if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+3]==0)
							ULONGDATA(birth)[6*pix+3]=l;

						if(( (l-ULONGDATA(birth)[6*pix+3]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXY, rs, ps)) )
							if((UCHARDATA(inhib)[pix]&CC_FXY)==0)
								cca_add_complexface(inhib, CC_FXY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+4]==0)
							ULONGDATA(birth)[6*pix+4]=l;

						if(( (l-ULONGDATA(birth)[6*pix+4]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXZ, rs, ps)) )
							if((UCHARDATA(inhib)[pix]&CC_FXZ)==0)
								cca_add_complexface(inhib, CC_FXZ, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+5]==0)
							ULONGDATA(birth)[6*pix+5]=l;

						if(( (l-ULONGDATA(birth)[6*pix+5]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FYZ, rs, ps)) )
							if((UCHARDATA(inhib)[pix]&CC_FYZ)==0)
								cca_add_complexface(inhib, CC_FYZ, pix, rs, ps);
					}


					pix++;
				}


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


	if(argc==3)
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
		inhib=readimage(argv[3]);
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

		free_faces=cca_collapse_directional_surfacic(image, inhib, 1, free_faces, &t1);
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

						if(( (l-ULONGDATA(birth)[6*pix]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AX, rs, ps))) )  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
							if((UCHARDATA(inhib)[pix]&CC_AX)==0)
								cca_add_complexface(inhib, CC_AX, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[6*pix+1]==0)
							ULONGDATA(birth)[6*pix+1]=l;

						if(( (l-ULONGDATA(birth)[6*pix+1]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AY, rs, ps))) ) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
							if((UCHARDATA(inhib)[pix]&CC_AY)==0)
								cca_add_complexface(inhib, CC_AY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[6*pix+2]==0)
							ULONGDATA(birth)[6*pix+2]=l;

						if(( (l-ULONGDATA(birth)[6*pix+2]) > (cca_getfacedist(image, filtermap_curv, i, j, k, CC_AZ, rs, ps))) ) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
							if((UCHARDATA(inhib)[pix]&CC_AZ)==0)
								cca_add_complexface(inhib, CC_AZ, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+3]==0)
							ULONGDATA(birth)[6*pix+3]=l;

						if(( (l-ULONGDATA(birth)[6*pix+3]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXY, rs, ps)) )
							if((UCHARDATA(inhib)[pix]&CC_FXY)==0)
								cca_add_complexface(inhib, CC_FXY, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+4]==0)
							ULONGDATA(birth)[6*pix+4]=l;

						if(( (l-ULONGDATA(birth)[6*pix+4]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FXZ, rs, ps)) )
							if((UCHARDATA(inhib)[pix]&CC_FXZ)==0)
								cca_add_complexface(inhib, CC_FXZ, pix, rs, ps);
					}

					if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
					{
						if(ULONGDATA(birth)[6*pix+5]==0)
							ULONGDATA(birth)[6*pix+5]=l;

						if(( (l-ULONGDATA(birth)[6*pix+5]) > cca_getfacedist(image, filtermap_surf, i, j, k, CC_FYZ, rs, ps)) )
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


	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhib);
	freeimage(birth);
	freeimage(filtermap_curv);
	freeimage(filtermap_surf);
	freeimage(t1);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}




