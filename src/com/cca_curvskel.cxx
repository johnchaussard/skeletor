
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
#include <liblist.h>

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
		//memcpy(distmap_in->image_data, t1->image_data, N*sizeof(unsigned long));
		for(i=0; i<N; i++)
			ULONGDATA(t2)[i] = ULONGDATA(t1)[i];
		freeimage(t1);
	}
	else
		freeimage(t2);

	return(0);
}




int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *filtermap, *distmap, *t1, *inhib, *birth;
	uint32_t N, i, j, k, d, rs, cs, ps, cont, l, x, y, z, decal, jmax;
	list **free_faces, *border_faces;
	face_desc fd, f;
	unsigned char face, dir;
	cell *c;

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
	//**********************************************
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
			freeimage(image);
			return(-1);
		}

		for(i=0; i<N; i++)
			ULONGDATA(filtermap)[i]=ULONGDATA(filtermap)[i]-ULONGDATA(distmap)[i];

		freeimage(distmap);
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
	border_faces=NULL;

	fprintf(stdout, "go\n");

	while(cont==1)
	{
		cont=0;

		free_faces=cca_collapse_directional_surfacic(image, inhib, 1, free_faces, &t1, &border_faces);
		if(free_faces==NULL)
		{
			fprintf(stderr, "Error during collapse.\n");
			return(-1);
		}
		l++;


		//Check all faces in the border : these face's configuration have changed since last collapse, and we may have to update their birth info
		while(!list_isempty(border_faces))
		{
			face=list_pop_uint8_t(border_faces);
			i=list_pop_uint32_t(border_faces);
			decal=0;

			jmax=cca_get_cardinal_subface(face);

			for(j=0; j<jmax; j++)
			{
				cca_get_subface(face, i, j, &fd, rs, ps);

				if(fd.face_type >= CC_AZ && fd.face_type <= CC_AX)
				{
					switch(fd.face_type)
					{
						case(CC_AZ):
							decal=2;
							break;

						case(CC_AY):
							decal=1;
							break;

						case(CC_AX):
						default:
							decal=0;
							break;
					}


					if((UCHARDATA(image)[fd.pixnumber] & fd.face_type)!=0)
					{
						x=getxfrompixnum(fd.pixnumber, rs, ps);
						y=getyfrompixnum(fd.pixnumber, rs, ps);
						z=getzfrompixnum(fd.pixnumber, rs, ps);

						if(	(cca_count_face_container(image, fd.pixnumber, x, y, z, fd.face_type, rs, ps)==0 ) &&
							(ULONGDATA(birth)[3*fd.pixnumber+decal]==0)	)
							ULONGDATA(birth)[3*fd.pixnumber+decal]=l;
					}
				}
			}
		}


		//Add in constraint set all edge not contained in any other face and that could be erased this turn (mean, one of its vertex is free)
		//Check therefore free vertices
		for(k=0; k<=5; k++)
		{
			if(!list_isempty(free_faces[k]))
			{
				face=cca_get_face_type_from_list_index(k);
				dir=cca_get_collapse_direction_from_list_index(k);

				//Just do this in order to get face type container associated with this collapse direction
				f=cca_get_container_of_free_face(face, 0, dir, rs, ps);
				switch(f.face_type)
				{
					case(CC_AZ):
						decal=2;
						break;

					case(CC_AY):
						decal=1;
						break;

					default:
						decal=0;
						break;
				}

				c=list_gethead(free_faces[k]);

				while(c!=NULL)
				{
					i=list_read_uint32_t(free_faces[k], c);
					f=cca_get_container_of_free_face(face, i, dir, rs, ps);

					if((UCHARDATA(image)[f.pixnumber]&f.face_type)!=0)
					{
						x=getxfrompixnum(f.pixnumber, rs, ps);
						y=getyfrompixnum(f.pixnumber, rs, ps);
						z=getzfrompixnum(f.pixnumber, rs, ps);

						if(	(l > (cca_getfacedist(image, filtermap, x, y, z, f.face_type, rs, ps) + 2*ULONGDATA(birth)[3*f.pixnumber+decal] - cca_getfacedist(image, distmap, x, y, z, f.face_type, rs, ps)) ) &&
							(UCHARDATA(inhib)[f.pixnumber]&f.face_type)==0	)
						{
							cca_add_complexface(inhib, f.face_type, f.pixnumber, rs, ps);
						}
					}

					c=list_gonext(c);
				}
			}



			/*if(!list_isempty(free_faces[k]))
			{
				face=cca_get_face_type_from_list_index(k);
				dir=cca_get_collapse_direction_from_list_index(k);

				c=list_gethead(free_faces[k]);

				while(c!=NULL)
				{
					i=list_read_uint32_t(free_faces[k], c);
					f=cca_get_container_of_free_face(face, i, dir, rs, ps);

					jmax=cca_get_cardinal_subface(f.face_type);
					decal=0;
					for(j=0; j<=jmax; j++)
					{
						if(j<jmax) //Just a little trick, in order to get the face itself
							cca_get_subface(f.face_type, i, j, &fd, rs, ps);
						else
							fd=f;

						//if(j==10) decal++;
						//else if(j==14) decal++;
						if(fd.face_type >= CC_AZ && fd.face_type <=CC_AX)
						{
							switch(fd.face_type)
							{
								case(CC_AZ):
									decal=2;
									break;

								case(CC_AY):
									decal=1;
									break;

								default:
									decal=0;
									break;
							}

							if((UCHARDATA(image)[fd.pixnumber]&fd.face_type)!=0)
							{
								x=getxfrompixnum(fd.pixnumber, rs, ps);
								y=getyfrompixnum(fd.pixnumber, rs, ps);
								z=getzfrompixnum(fd.pixnumber, rs, ps);

								if(	(cca_count_face_container(image, fd.pixnumber, x, y, z, fd.face_type, rs, ps)==0 ) &&
									(l > 2*(cca_getfacedist(image, filtermap, x, y, z, fd.face_type, rs, ps) + ULONGDATA(birth)[3*fd.pixnumber+decal]) ) &&
									(UCHARDATA(inhib)[fd.pixnumber]&fd.face_type)==0	)
								{
									cca_add_complexface(inhib, fd.face_type, fd.pixnumber, rs, ps);
								}
							}
						}
					}

					c=list_gonext(c);
				}
			}*/
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

	list_delete(border_faces, NO_FREE_DATA);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}



