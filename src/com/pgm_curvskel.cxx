
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


#define USAGE "<input_pgm> <inhib_pgm|NULL> <output_pgm>"


uint32_t cca_count_edge_in_cube(struct xvimage *cca_image, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	uint32_t r=0;

	if((UCHARDATA(cca_image)[pixnumber] & CC_AX) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+rs] & CC_AX) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+ps] & CC_AX) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+rs+ps] & CC_AX) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber] & CC_AY) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+1] & CC_AY) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+ps] & CC_AY) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+1+ps] & CC_AY) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber] & CC_AZ) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+1] & CC_AZ) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+rs] & CC_AZ) != 0) r++;
	if((UCHARDATA(cca_image)[pixnumber+1+rs] & CC_AZ) != 0) r++;

	return r;
}

uint32_t pgm_count_direct_neighbours(struct xvimage *pgm_image, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	uint32_t r, x, y, z;

	r=0;
	x=getxfrompixnum(pixnumber, rs, ps);
	y=getyfrompixnum(pixnumber, rs, ps);
	z=getzfrompixnum(pixnumber, rs, ps);


	if( (x>0) && (UCHARDATA(pgm_image)[pixnumber-1]!=0)) r++;
	if( (x<(uint32_t)rowsize(pgm_image)) && (UCHARDATA(pgm_image)[pixnumber+1]!=0)) r++;

	if( (y>0) && (UCHARDATA(pgm_image)[pixnumber-rs]!=0)) r++;
	if( (y<(uint32_t)colsize(pgm_image)) && (UCHARDATA(pgm_image)[pixnumber+rs]!=0)) r++;

	if( (z>0) && (pixnumber>=ps) && (UCHARDATA(pgm_image)[pixnumber-ps]!=0)) r++;
	if( (z<(uint32_t)depth(pgm_image)) && (depth(pgm_image)>1) && (UCHARDATA(pgm_image)[pixnumber+ps]!=0)) r++;

	return r;
}


uint32_t cca_highest_value(struct xvimage *cca_image, struct xvimage *distmap, uint32_t i, uint32_t j, uint32_t k, unsigned char face_type, uint32_t rs, uint32_t ps, struct xvimage* output)
{
	uint32_t pix, bord, newps, newrs, max, ccpix, ccmax, max_count, cctest, test, test_count;
	uint32_t maxneigh, testneigh;

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
			fprintf(stderr, "cca_highest_value(): Wrong face type given.\n");
	}

	ccpix=i+j*rs+k*ps;

	switch(face_type)
	{
		case(CC_AX):
			ccmax=ccpix;
			max=pix;
			max_count=cca_count_edge_in_cube(cca_image, ccmax, rs, ps);
			maxneigh=pgm_count_direct_neighbours(output, max, newrs, newps);


			cctest=ccpix-rs;
			test=pix-newrs;
			test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
			testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
			if((bord & Y_BORDER_MIN) == 0)
			{
				if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
				{
					max=test;
					ccmax=cctest;
					max_count=test_count;
					maxneigh=testneigh;
				}
				else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
				{
					if(	(test_count > max_count) ||
						( (test_count == max_count) && (testneigh > maxneigh) ) ||
						( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
				}
			}


			if (ccpix >= ps) //En 2d, ceci devrait éviter un probleme...
			{
				cctest=ccpix-ps;
				test=pix-newps;
				test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
				testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
				if((bord & Z_BORDER_MIN) == 0)
				{
					if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
					else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
					{
						if(	(test_count > max_count) ||
							( (test_count == max_count) && (testneigh > maxneigh) ) ||
							( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
						{
							max=test;
							ccmax=cctest;
							max_count=test_count;
							maxneigh=testneigh;
						}
					}
				}
			}


			if (ccpix >= ps + rs)
			{
				cctest=ccpix-ps-rs;
				test=pix-newps-newrs;
				test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
				testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
				if( ((bord & Z_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) )
				{
					if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
					else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
					{
						if(	(test_count > max_count) ||
							( (test_count == max_count) && (testneigh > maxneigh) ) ||
							( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
						{
							max=test;
							ccmax=cctest;
							max_count=test_count;
							maxneigh=testneigh;
						}
					}
				}
			}

			//cca_remove_complexface(cca_image, CC_VOL, ccmax, rs, ps);

			return(max);
			break;




		case(CC_AY):
			ccmax=ccpix;
			max=pix;
			max_count=cca_count_edge_in_cube(cca_image, ccmax, rs, ps);
			maxneigh=pgm_count_direct_neighbours(output, max, newrs, newps);


			cctest=ccpix-1;
			test=pix-1;
			test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
			testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
			if((bord & X_BORDER_MIN) == 0)
			{
				if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
				{
					max=test;
					ccmax=cctest;
					max_count=test_count;
					maxneigh=testneigh;
				}
				else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
				{
					if(	(test_count > max_count) ||
						( (test_count == max_count) && (testneigh > maxneigh) ) ||
						( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
				}
			}


			if (ccpix >= ps) //En 2d, ceci devrait éviter un probleme...
			{
				cctest=ccpix-ps;
				test=pix-newps;
				test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
				testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
				if((bord & Z_BORDER_MIN) == 0)
				{
					if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
					else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
					{
						if(	(test_count > max_count) ||
							( (test_count == max_count) && (testneigh > maxneigh) ) ||
							( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
						{
							max=test;
							ccmax=cctest;
							max_count=test_count;
							maxneigh=testneigh;
						}
					}
				}
			}


			if (ccpix >= ps +1) //En 2d, ceci devrait éviter un probleme...
			{
				cctest=ccpix-ps-1;
				test=pix-newps-1;
				test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
				testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
				if( ((bord & Z_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) )
				{
					if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
					else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
					{
						if(	(test_count > max_count) ||
							( (test_count == max_count) && (testneigh > maxneigh) ) ||
							( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
						{
							max=test;
							ccmax=cctest;
							max_count=test_count;
							maxneigh=testneigh;
						}
					}
				}
			}

			//cca_remove_complexface(cca_image, CC_VOL, ccmax, rs, ps);

			return(max);
			break;


		case(CC_AZ):
			ccmax=ccpix;
			max=pix;
			max_count=cca_count_edge_in_cube(cca_image, ccmax, rs, ps);
			maxneigh=pgm_count_direct_neighbours(output, max, newrs, newps);


			cctest=ccpix-1;
			test=pix-1;
			test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
			testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
			if((bord & X_BORDER_MIN) == 0)
			{
				if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
				{
					max=test;
					ccmax=cctest;
					max_count=test_count;
					maxneigh=testneigh;
				}
				else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
				{
					if(	(test_count > max_count) ||
						( (test_count == max_count) && (testneigh > maxneigh) ) ||
						( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
				}
			}


			cctest=ccpix-rs;
			test=pix-newrs;
			test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
			testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
			if((bord & Y_BORDER_MIN) == 0)
			{
				if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
				{
					max=test;
					ccmax=cctest;
					max_count=test_count;
					maxneigh=testneigh;
				}
				else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
				{
					if(	(test_count > max_count) ||
						( (test_count == max_count) && (testneigh > maxneigh) ) ||
						( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
				}
			}


			cctest=ccpix-rs-1;
			test=pix-newrs-1;
			test_count=cca_count_edge_in_cube(cca_image, cctest, rs, ps);
			testneigh=pgm_count_direct_neighbours(output, test, newrs, newps);
			if( ((bord & Y_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) )
			{
				if(ULONGDATA(distmap)[max]==0 && ULONGDATA(distmap)[test]>0)
				{
					max=test;
					ccmax=cctest;
					max_count=test_count;
					maxneigh=testneigh;
				}
				else if(ULONGDATA(distmap)[max]>0 && ULONGDATA(distmap)[test]>0)
				{
					if(	(test_count > max_count) ||
						( (test_count == max_count) && (testneigh > maxneigh) ) ||
						( (test_count == max_count) && (testneigh == maxneigh) && (ULONGDATA(distmap)[max] < ULONGDATA(distmap)[test])) )
					{
						max=test;
						ccmax=cctest;
						max_count=test_count;
						maxneigh=testneigh;
					}
				}
			}

			//cca_remove_complexface(cca_image, CC_VOL, ccmax, rs, ps);

			return(max);
			break;
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



		default:
			fprintf(stderr, "cca_highest_value(): Wrong face type given (2).\n");
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
	struct xvimage *image, *filtermap, *distmap, *t1, *inhib, *birth, *pgm_map, *output;
	uint32_t N, i, j, k, d, rs, cs, ps, pix, cont, l, pgm_cpt, v;
	list **free_faces;

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
		fprintf(stderr, "Error: only 1 byte per pixel PGM image supported\n");
		return(-1);
	}

	rs=rowsize(image);
	ps=rs*colsize(image);

	d=depth(image);
	N=rowsize(image)*colsize(image)*d;

	pgm_map=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_4_BYTE);
	pgm_cpt=1;

	output=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);



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
			ULONGDATA(filtermap)[i]=(ULONGDATA(filtermap)[i]-ULONGDATA(distmap)[i]);

		freeimage(distmap);
		//distmap=NULL;
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


	if (strcmp(argv[2], "NULL")==0)
	{
		inhib=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
		if(inhib==NULL)
		{
			fprintf(stderr, "Error: memory allocation failed for inhibit image.\n");
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}
	}
	else
	{
		inhib=readimage(argv[2]);
		if(inhib==NULL)
		{
			fprintf(stderr, "Error: could not open image %s.\n", argv[2]);
			freeimage(filtermap);
			freeimage(image);
			return(-1);
		}
		pgm_to_cca(inhib);
		writeimage(inhib, "inhib.cca");

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
						//Paramètre de filtrage ajustable, scal invariant...
						if(l  > ((cca_getfacedist(image, filtermap, i, j, k, CC_AX, rs, ps) + 2*ULONGDATA(birth)[3*pix]) *1))  //cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix]))
							if((UCHARDATA(inhib)[pix]&CC_AX)==0)
							{
								cca_add_complexface(inhib, CC_AX, pix, rs, ps);
							}
					}

					if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+1]==0)
							ULONGDATA(birth)[3*pix+1]=l;



						if(l > ((cca_getfacedist(image, filtermap, i, j, k, CC_AY, rs, ps) + 2*ULONGDATA(birth)[3*pix+1]) *1)) // cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+1]))
							if((UCHARDATA(inhib)[pix]&CC_AY)==0)
							{
								cca_add_complexface(inhib, CC_AY, pix, rs, ps);
							}
					}

					if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
					{
						if(ULONGDATA(birth)[3*pix+2]==0)
							ULONGDATA(birth)[3*pix+2]=l;



						if(l  > ((cca_getfacedist(image, filtermap, i, j, k, CC_AZ, rs, ps) + 2*ULONGDATA(birth)[3*pix+2]) *1)) //cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)+ noise  <= (l-ULONGDATA(birth)[3*pix+2]))
							if((UCHARDATA(inhib)[pix]&CC_AZ)==0)
							{
								cca_add_complexface(inhib, CC_AZ, pix, rs, ps);
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


	cca_keep_only_cells(image);



	/*maxvalue = pgm_cpt-1;

	fprintf(stdout, "hey %d\n", maxvalue);

	tabvalues = (uint32_t*)calloc(maxvalue+1, sizeof(uint32_t));
	N=rowsize(pgm_map)*colsize(pgm_map)*depth(pgm_map);
	cpt=0;
	for(i=0; i<N; i++)
	{
		if(ULONGDATA(pgm_map)[i] > 0)
		{
				tabvalues[ULONGDATA(pgm_map)[i]]=i;
				cpt++;
		}
	}


	fprintf(stdout, "hey %d\n", cpt);

	rspgm=rs-1;
	cspgm=cs-1;
	pspgm=rspgm*cspgm;
	for(cpt=maxvalue; cpt>0; cpt--)
	{
		pix=tabvalues[cpt];
		i=getxfrompixnum(pix, rspgm, pspgm);
		j=getyfrompixnum(pix, rspgm, pspgm);
		k=getzfrompixnum(pix, rspgm, pspgm);

		pixnumber=i+j*rs+k*ps;
		if(	((UCHARDATA(image)[pixnumber] & CC_AX) != 0) ||
			((UCHARDATA(image)[pixnumber+rs] & CC_AX) != 0) ||
			((UCHARDATA(image)[pixnumber+ps] & CC_AX) != 0) ||
			((UCHARDATA(image)[pixnumber+rs+ps] & CC_AX) != 0) ||
			((UCHARDATA(image)[pixnumber] & CC_AY) != 0) ||
			((UCHARDATA(image)[pixnumber+1] & CC_AY) != 0) ||
			((UCHARDATA(image)[pixnumber+ps] & CC_AY) != 0) ||
			((UCHARDATA(image)[pixnumber+1+ps] & CC_AY) != 0) ||
			((UCHARDATA(image)[pixnumber] & CC_AZ) != 0) ||
			((UCHARDATA(image)[pixnumber+1] & CC_AZ) != 0) ||
			((UCHARDATA(image)[pixnumber+rs] & CC_AZ) != 0) ||
			((UCHARDATA(image)[pixnumber+1+rs] & CC_AZ) != 0) )
		{
			cca_remove_complexface(image, CC_VOL, pixnumber, rs, ps);
			UCHARDATA(output)[pix]=255;
		}
	}*/



	pix=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if((UCHARDATA(image)[pix]&CC_FXY)!=0)  //En 2d, il peut rester des volumes à cause de l'inhibiteur
				{
					v=cca_highest_value(image, pgm_map, i, j, k, CC_FXY, rs, ps, output);
					UCHARDATA(output)[v]=255;
				}



				if((UCHARDATA(image)[pix]&CC_AX)!=0)
				{
					v=cca_highest_value(image, pgm_map, i, j, k, CC_AX, rs, ps, output);
					UCHARDATA(output)[v]=255;
				}

				if((UCHARDATA(image)[pix]&CC_AY)!=0)
				{
					v=cca_highest_value(image, pgm_map, i, j, k, CC_AY, rs, ps, output);
					UCHARDATA(output)[v]=255;
				}

				if((UCHARDATA(image)[pix]&CC_AZ)!=0)
				{
					v=cca_highest_value(image, pgm_map, i, j, k, CC_AZ, rs, ps, output);
					UCHARDATA(output)[v]=255;
				}

				/*if((UCHARDATA(image)[pix]&CC_PT)!=0)
				{
					UCHARDATA(output)[cca_highest_value(image, pgm_map, i, j, k, CC_PT, rs, ps, output)]=255;
				}*/



				pix++;
			}






	writeimage(output, argv[3]);




	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhib);
	freeimage(birth);
	freeimage(filtermap);
	freeimage(t1);

	for(i=0; i<24; i++)
	{
		list_delete(free_faces[i],NO_FREE_DATA);
	}
	free(free_faces);



	return(0);
}




