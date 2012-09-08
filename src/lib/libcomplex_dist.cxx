//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>
#include <mcimage.h>

//Librairie perso
#include <libcomplex.h>


#define UINT32_T_MAX 4294967295




void geodilat26(struct xvimage* values, struct xvimage *mask)
{
	uint32_t pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, i, p;
	int32_t x, y, z, a, b, c;
	int32_t k;
	list *l;


	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;

	l=list_newlist(MODE_FIFO, 1000);
	if(l==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for list.\n");
		assert(0);
	}


	for(i=0; i<pgm_N; i++)
	{
		if( (ULONGDATA(values)[i]!=0) )
		{
			list_push_uint32_t(l, i);
		}
	}



	while(!list_isempty(l))
	{
		i= list_pop_uint32_t(l);
		x = i%pgm_rs;
		z = i/pgm_ps;
		y = (i%pgm_ps)/pgm_rs;

		for(a=-1; a<=1; a++)
		for(b=-1; b<=1; b++)
		for(c=-1; c<=1; c++)
		{
			//if( (a==0 && b==0) || (a==0 && c==0) || (b==0 && c==0) ) //Six voisins
			if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
			{
				p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
				if(UCHARDATA(mask)[p] != 0 && ULONGDATA(values)[p] == 0)
				{
					ULONGDATA(values)[p]=ULONGDATA(values)[i];
					list_push_uint32_t(l, p);
				}
			}
		}
	}
}


void geodilat6_with_radius_limit(struct xvimage* values, struct xvimage *mask, struct xvimage *distc6)
{
	uint32_t pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, i, p, cpt, turn, number_for_current_turn;
	int32_t x, y, z, a, b, c;
	int32_t k;
	list *l;


	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;

	l=list_newlist(MODE_FIFO, 1000);
	if(l==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for list.\n");
		assert(0);
	}


	for(i=0; i<pgm_N; i++)
	{
		if( (ULONGDATA(values)[i]!=0) )
		{
			list_push_uint32_t(l, i);
		}
	}


	turn=1;
	number_for_current_turn = list_getsize(l);
	cpt=0;


	while(!list_isempty(l))
	{
		if(cpt==number_for_current_turn)
		{
			cpt=0;
			turn++;
			number_for_current_turn = list_getsize(l);
		}

		cpt++;

		i= list_pop_uint32_t(l);
		x = i%pgm_rs;
		z = i/pgm_ps;
		y = (i%pgm_ps)/pgm_rs;

		if(ULONGDATA(distc6)[i] >= turn)
		{

			for(a=-1; a<=1; a++)
			for(b=-1; b<=1; b++)
			for(c=-1; c<=1; c++)
			{
				if( (a==0 && b==0) || (a==0 && c==0) || (b==0 && c==0) ) //Six voisins
				if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
				{
					p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
					if(UCHARDATA(mask)[p] != 0 && ULONGDATA(values)[p] == 0)
					{
						ULONGDATA(values)[p]=ULONGDATA(values)[i];
						//if(p==23238882)
						//	fprintf(stdout, "%lu\n", ULONGDATA(values)[72+89*pgm_rs+359*pgm_ps]);
						list_push_uint32_t(l, p);
					}
				}
			}
		}
	}
}



void geodilat26_with_radius_limit(struct xvimage* values, struct xvimage *mask, struct xvimage *distc6)
{
	uint32_t pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, i, p, cpt, turn, number_for_current_turn;
	int32_t x, y, z, a, b, c;
	int32_t k;
	list *l;


	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;

	l=list_newlist(MODE_FIFO, 1000);
	if(l==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for list.\n");
		assert(0);
	}


	for(i=0; i<pgm_N; i++)
	{
		if( (ULONGDATA(values)[i]!=0) )
		{
			list_push_uint32_t(l, i);
		}
	}


	turn=1;
	number_for_current_turn = list_getsize(l);
	cpt=0;


	while(!list_isempty(l))
	{
		if(cpt==number_for_current_turn)
		{
			cpt=0;
			turn++;
			number_for_current_turn = list_getsize(l);
		}

		cpt++;

		i= list_pop_uint32_t(l);
		x = i%pgm_rs;
		z = i/pgm_ps;
		y = (i%pgm_ps)/pgm_rs;

		if(ULONGDATA(distc6)[i] >= turn)
		{

			for(a=-1; a<=1; a++)
			for(b=-1; b<=1; b++)
			for(c=-1; c<=1; c++)
			{
				//if( (a==0 && b==0) || (a==0 && c==0) || (b==0 && c==0) ) //Six voisins
				if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
				{
					p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
					if(UCHARDATA(mask)[p] != 0 && ULONGDATA(values)[p] == 0)
					{
						ULONGDATA(values)[p]=ULONGDATA(values)[i];
						//if(p==23238882)
						//	fprintf(stdout, "%lu\n", ULONGDATA(values)[72+89*pgm_rs+359*pgm_ps]);
						list_push_uint32_t(l, p);
					}
				}
			}
		}
	}
}



void geodilat26euc_with_radius_limit(struct xvimage* values, struct xvimage *mask, struct xvimage *distc6)
{
	uint32_t pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, i, p, cpt, turn, number_for_current_turn;
	int32_t x, y, z, a, b, c, xc, yc, zc;
	int32_t k;
	list *l;


	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;

	l=list_newlist(MODE_FIFO, 1000);
	if(l==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for list.\n");
		assert(0);
	}


	for(i=0; i<pgm_N; i++)
	{
		if( (ULONGDATA(values)[i]!=0) )
		{
			list_push_uint32_t(l, i);
		}
	}



	while(!list_isempty(l))
	{
		i= list_pop_uint32_t(l);
		x = i%pgm_rs;
		z = i/pgm_ps;
		y = (i%pgm_ps)/pgm_rs;

		xc = (ULONGDATA(values)[i])%pgm_rs;
		zc = (ULONGDATA(values)[i])/pgm_ps;
		yc = ((ULONGDATA(values)[i])%pgm_ps)/pgm_rs;

		if(ULONGDATA(distc6)[ULONGDATA(values)[i]] >= ((xc-x)*(xc-x) + (yc-y)*(yc-y) + (zc-z)*(zc-z)))
		{
			for(a=-1; a<=1; a++)
			for(b=-1; b<=1; b++)
			for(c=-1; c<=1; c++)
			{
				//if( (a==0 && b==0) || (a==0 && c==0) || (b==0 && c==0) ) //Six voisins
				if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
				{
					p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
					if(UCHARDATA(mask)[p] != 0 && ULONGDATA(values)[p] == 0)
					{
						ULONGDATA(values)[p]=ULONGDATA(values)[i];
						//if(p==23238882)
						//	fprintf(stdout, "%lu\n", ULONGDATA(values)[72+89*pgm_rs+359*pgm_ps]);
						list_push_uint32_t(l, p);
					}
				}
			}
		}
	}
}


void geodilat6(struct xvimage* values, struct xvimage *mask)
{
	uint32_t pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, i, p;
	int32_t x, y, z, a, b, c;
	int32_t k;
	list *l;


	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;

	l=list_newlist(MODE_FIFO, 1000);
	if(l==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for list.\n");
		assert(0);
	}


	for(i=0; i<pgm_N; i++)
	{
		if( (ULONGDATA(values)[i]!=0) )
		{
			list_push_uint32_t(l, i);
		}
	}



	while(!list_isempty(l))
	{
		i= list_pop_uint32_t(l);
		x = i%pgm_rs;
		z = i/pgm_ps;
		y = (i%pgm_ps)/pgm_rs;

		for(a=-1; a<=1; a++)
		for(b=-1; b<=1; b++)
		for(c=-1; c<=1; c++)
		{
			if( (a==0 && b==0) || (a==0 && c==0) || (b==0 && c==0) ) //Six voisins
			if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
			{
				p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
				if(UCHARDATA(mask)[p] != 0 && ULONGDATA(values)[p] == 0)
				{
					ULONGDATA(values)[p]=ULONGDATA(values)[i];
					//if(p==23238882)
					//	fprintf(stdout, "%lu\n", ULONGDATA(values)[72+89*pgm_rs+359*pgm_ps]);
					list_push_uint32_t(l, p);
				}
			}
		}
	}

	//fprintf(stdout, "%lu\n", ULONGDATA(values)[72+89*pgm_rs+359*pgm_ps]);
}


struct xvimage *compute_closest_cca_vertex_to_pgm(struct xvimage *cca_image, struct xvimage *mask, struct xvimage *border, struct xvimage *distc6)
{
	uint32_t j, cpt_vertex, cca_rs, cca_cs, cca_N, cca_ps, cca_d, pgm_rs, pgm_cs, pgm_d, pgm_N, pgm_ps, x, y, z, i, p, a, b, c;
	struct xvimage *result;
	int32_t k;
	bool found;



	if(datatype(cca_image)!=VFF_TYP_1_BYTE || datatype(mask)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Input images must be char images.\n");
		return(NULL);
	}


	cca_rs = rowsize(cca_image);
	cca_cs = colsize(cca_image);
	cca_d = depth(cca_image);
	cca_ps = cca_rs * cca_cs;
	cca_N = cca_ps * cca_d;

	pgm_rs = rowsize(mask);
	pgm_cs = colsize(mask);
	pgm_d = depth(mask);
	pgm_ps = pgm_rs * pgm_cs;
	pgm_N = pgm_ps * pgm_d;





	if(pgm_rs+1 != cca_rs  ||  pgm_cs+1 != cca_cs  ||  pgm_d+1 != cca_d)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Input images dont't have compatible size.\n");
		return(NULL);
	}


	result=allocimage(NULL, pgm_rs, pgm_cs, pgm_d, VFF_TYP_4_BYTE);
	if(result==NULL)
	{
		fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Memory allocation error for output image.\n");
		return(NULL);
	}

	cpt_vertex=0;
	for(i=0; i<cca_N; i++)
	{
		if( (UCHARDATA(cca_image)[i] & CC_PT) !=0)
		{
			cpt_vertex++;
			x = i%cca_rs;
			z = i/cca_ps;
			y = (i%cca_ps)/cca_rs;

			//if(cpt_vertex==18241) fprintf(stdout, "%d %d %d\n", x, y, z);

			for(a=0; a<=1; a++)
			for(b=0; b<=1; b++)
			for(c=0; c<=1; c++)
			{
				if( (x-a)<pgm_rs  &&  (y-b)<pgm_cs  &&  (z-c)<pgm_d  &&  (x-a)>=0  &&  (y-b)>=0  &&  (z-c)>=0 )
				{
					p = (x-a) + (y-b)*pgm_rs + (z-c)*pgm_ps;
					if(UCHARDATA(mask)[p] != 0 && ULONGDATA(result)[p] == 0)
					{
						ULONGDATA(result)[p]=cpt_vertex;
					}
				}
			}
		}
	}


	geodilat26euc_with_radius_limit(result, mask, distc6);
	geodilat6(result, mask);
	geodilat26(result, mask);
	//fprintf(stdout, "%lu\n", ULONGDATA(result)[73+89*pgm_rs+359*pgm_ps]);
	//fprintf(stdout, "%lu\n", UCHARDATA(border)[72+89*pgm_rs+359*pgm_ps]);
	geodilat6(result, border);
	geodilat26(result, border);





	for(i=0; i<pgm_N; i++)
	{
		//ULONGDATA(result)[i]=0;    //DEBUG

		//For each point, check the 26-neihbourhood and take the max value from points inside the mask, and 0 else (no point inside max in the 26-neighbourhood)
		if( (UCHARDATA(mask)[i] == 0) && (UCHARDATA(border)[i] == 0) )
		{
//			found=false;
//			for(j=0; j<26; j++)
//			{
//				k=voisin26(i, j, pgm_rs, pgm_ps, pgm_N);
//				if(k>=0 && UCHARDATA(mask)[k]!=0)
//				{
//					ULONGDATA(result)[i] = ULONGDATA(result)[k];
//					found=true;
//					break;
//				}
//			}
//
//			if(!found) ULONGDATA(result)[i] = UINT32_T_MAX;

		ULONGDATA(result)[i] = UINT32_T_MAX;
		}
		else
		{
			if(ULONGDATA(result)[i]==0)
			{
				//fprintf(stderr, "compute_closest_cca_vertex_to_pgm() : Error, pixel %u does not have any value.\nThis may be because object is 26-connected and distance used is 6-distance.\n", i);
				//freeimage(result);
				//list_delete(l, NO_FREE_DATA);
				//return(NULL);
			}
			ULONGDATA(result)[i] = ULONGDATA(result)[i] -1;
		}
	}

	//fprintf(stdout, "%lu\n", UCHARDATA(border)[72+89*pgm_rs+359*pgm_ps]);
	//fprintf(stdout, "%lu\n", ULONGDATA(result)[72+89*pgm_rs+359*pgm_ps]);
	fprintf(stdout, "%lu\n", ULONGDATA(result)[132+93*pgm_rs+358*pgm_ps]);


	//DEBUG
	//fprintf(stdout, "%lu\n", ULONGDATA(result)[127+119*pgm_rs+314*pgm_ps]);
	//ULONGDATA(result)[47+147*pgm_rs+209*pgm_ps]=255;

	//list_delete(l, NO_FREE_DATA);
	return (result);
}
