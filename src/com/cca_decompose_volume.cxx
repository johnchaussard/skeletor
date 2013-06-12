/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libinventor.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<pgm_volume> <cca_skel> <cca_skel_main_faces> <pgm_label_output_image> (<geo_dilat_cca>)"


int32_t main(int argc, char *argv[])
{
	struct xvimage *cca_skel_main_faces, *cca_skel, *output_pgm_label, *pgm_volume, *geodilat, *pgm_latemap;
	list *decomposition, *only_surfaces, *only_curves, *list_current_pixels, *list_next_pixels, *tlist;
	complexe *point, *t;
	uint32_t i, rs_pgm, cs_pgm, d_pgm, rs_cca, cs_cca, d_cca, N_pgm, N_cca, ps_cca, ps_pgm, label, pix_cca, pix_pgm, x, y, z, nbpix, todilate, current_pixel, voisin;
	uint32_t *tab_pix;
	int32_t signed_count;


	//uint32_t turn=0;


	//*******************************************
	//Open skeleton image
	//*******************************************
	if (argc!=5 && argc!=6)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	cca_skel=readimage(argv[2]);
	if(cca_skel==NULL)
	{
		fprintf(stderr, "Could not open image %s\n", argv[2]);
		return(1);
	}

	rs_cca=rowsize(cca_skel);
	cs_cca=colsize(cca_skel);
	d_cca=depth(cca_skel);
	ps_cca=rs_cca*cs_cca;
	N_cca=ps_cca*d_cca;


	//********************************************************
	//Make skeleton decomposition
	//********************************************************

	//The image will be changed by the decomposition function, so we first save the points into a complex
	point=complexe_new_complexe();
	if(point==NULL)
	{
		fprintf(stderr, "Could not create new complex structure.\n");
		freeimage(cca_skel);
		return(1);
	}

	if(cca_scan_all_vertex_into_complex(cca_skel, point) != 0)
	{
		fprintf(stderr, "Error in cca_scan_all_vertex_into_complex() function.\n");
		freeimage(cca_skel);
		complexe_free_complexe(point);
		return(1);
	}

	//We can perform decomposition
	decomposition=cca_decompose_skeleton_into_elements(cca_skel);
	if(decomposition==NULL)
	{
		fprintf(stderr, "Error in the decomposition function.\n");
		freeimage(cca_skel);
		complexe_free_complexe(point);
		return(1);
	}


	//Get rid of intersection elements
	t=(complexe*)list_pop_pointer(decomposition);
	complexe_free_complexe(t);


	//We don't need the skeleton image anymore
	freeimage(cca_skel);
	cca_skel=NULL;



	//*******************************************************
	//Should we geodilat the elements inside a higher level image
	//*******************************************************
	if(argc==6)
	{
		only_surfaces = list_newlist(MODE_FIFO, list_getsize(decomposition));
		only_curves = list_newlist(MODE_FIFO, list_getsize(decomposition));
		geodilat=readimage(argv[5]);
		complexe_free_complexe(point);
		//point=complexe_new_complexe();
		//cca_scan_all_vertex_into_complex(geodilat, point);

		while(!list_isempty(decomposition))
		{
			t=(complexe*)list_pop_pointer(decomposition);
			if(complexe_get_dimension(t)==2)
			{
				list_push_pointer(only_surfaces, t);
			}
			else
			{
				list_push_pointer(only_curves, t);
			}
		}

		//complexe_geodilat_inside_cca(only_surfaces, geodilat);

		list_append_to_other_list(only_surfaces, decomposition);
		list_append_to_other_list(only_curves, decomposition);

		list_delete(only_surfaces, NO_FREE_DATA);
		list_delete(only_curves, NO_FREE_DATA);

		freeimage(geodilat);
	}





	//************************************************
	//Open the other images
	//************************************************

	//Todo : main face could be opened, transformed into a complex, then freed, then image could be openend
	//main faces would be sent to image, and labeled by scanning the previous decomposition
	cca_skel_main_faces=readimage(argv[3]);
	if(cca_skel_main_faces==NULL)
	{
		fprintf(stderr, "Could not open image %s\n", argv[1]);
		return(1);
	}


	pgm_volume=readimage(argv[1]);
	if(pgm_volume==NULL)
	{
		fprintf(stderr, "Could not read image %s\n", argv[3]);
		return(1);
	}

	rs_pgm=rowsize(pgm_volume);
	cs_pgm=colsize(pgm_volume);
	d_pgm=depth(pgm_volume);
	ps_pgm=rs_pgm*cs_pgm;
	N_pgm=ps_pgm*d_pgm;


	if((rs_pgm!=rs_cca-1) || (cs_pgm!=cs_cca-1) || (d_pgm!=d_cca-1))
	{
		fprintf(stderr, "cc images and pgm images' sizes do not match\n");
		return(1);
	}


	//Todo : output and input should be same image, in order to save memory
	output_pgm_label=allocimage(NULL, rs_pgm, cs_pgm, d_pgm, VFF_TYP_4_BYTE);
	if(output_pgm_label==NULL)
	{
		fprintf(stderr, "Image allocation failed.\n");
		return(1);
	}

	for(i=0; i<N_pgm; i++)
		if(UCHARDATA(pgm_volume)[i] != 0)
			ULONGDATA(output_pgm_label)[i]=1;

	freeimage(pgm_volume);




	label=1;

	nbpix=0;

	while(!list_isempty(decomposition))
	{
		//Get the object to write
		t=(complexe*)list_pop_pointer(decomposition);

		label++;

		for(i=0; i<t->num_ax; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_ax[i]] & CC_AX) != 0)
			{
				pix_cca=t->tab_ax[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(y>0 && z>0 && ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm-ps_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm-ps_pgm]=label;
					nbpix++;
				}

				if(y>0 && z<d_pgm && ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]=label;
					nbpix++;
				}

				if(y<cs_pgm && z>0 && ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]=label;
					nbpix++;
				}

				if(y<cs_pgm && z<d_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}

			}
		}

		for(i=0; i<t->num_ay; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_ay[i]] & CC_AY) != 0)
			{
				pix_cca=t->tab_ay[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(x>0 && z>0 && ULONGDATA(output_pgm_label)[pix_pgm-1-ps_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-1-ps_pgm]=label;
					nbpix++;
				}

				if(x>0 && z<d_pgm && ULONGDATA(output_pgm_label)[pix_pgm-1]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-1]=label;
					nbpix++;
				}

				if(x<rs_pgm && z>0 && ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]=label;
					nbpix++;
				}

				if(x<rs_pgm && z<d_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}

			}
		}

		for(i=0; i<t->num_az; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_az[i]] & CC_AZ) != 0)
			{
				pix_cca=t->tab_az[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(y>0 && x>0 && ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm-1]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm-1]=label;
					nbpix++;
				}

				if(y>0 && x<rs_pgm && ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]=label;
					nbpix++;
				}

				if(y<cs_pgm && x>0 && ULONGDATA(output_pgm_label)[pix_pgm-1]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-1]=label;
					nbpix++;
				}

				if(y<cs_pgm && x<rs_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}

			}
		}

		for(i=0; i<t->num_fxy; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_fxy[i]] & CC_FXY) != 0)
			{
				pix_cca=t->tab_fxy[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(z>0 && ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-ps_pgm]=label;
					nbpix++;
				}

				if(z<d_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}
			}
		}

		for(i=0; i<t->num_fxz; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_fxz[i]] & CC_FXZ) != 0)
			{
				pix_cca=t->tab_fxz[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(y>0 && ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-rs_pgm]=label;
					nbpix++;
				}

				if(y<cs_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}
			}
		}

		for(i=0; i<t->num_fyz; i++)
		{
			if( (UCHARDATA(cca_skel_main_faces)[t->tab_fyz[i]] & CC_FYZ) != 0)
			{
				pix_cca=t->tab_fyz[i];
				x=getxfrompixnum(pix_cca, rs_cca, ps_cca);
				y=getyfrompixnum(pix_cca, rs_cca, ps_cca);
				z=getzfrompixnum(pix_cca, rs_cca, ps_cca);
				pix_pgm=x+y*rs_pgm+z*ps_pgm;

				if(x>0 && ULONGDATA(output_pgm_label)[pix_pgm-1]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm-1]=label;
					nbpix++;
				}

				if(x<rs_pgm && ULONGDATA(output_pgm_label)[pix_pgm]==1)
				{
					ULONGDATA(output_pgm_label)[pix_pgm]=label;
					nbpix++;
				}
			}
		}

		complexe_free_complexe(t);
	}

	list_delete(decomposition, NO_FREE_DATA);

	freeimage(cca_skel_main_faces);


	if(strcmp(argv[4], "NULL")!=0)
	{
		pgm_latemap=readimage(argv[4]);
		if(pgm_latemap==NULL)
		{
			fprintf(stderr, "Error, could not open image %s.\n", argv[4]);
			return(1);
		}
	}
	else
		pgm_latemap=NULL;


	if(pgm_latemap!=NULL)
	{
		tab_pix=(uint32_t*)calloc(nbpix, 2*sizeof(uint32_t));
		if(tab_pix==NULL)
		{
			fprintf(stderr, "Error: Memory allocation failed.\n");
			return(1);
		}

		nbpix=0;
		for(i=0; i<N_pgm; i++)
			if(ULONGDATA(output_pgm_label)[i]>1)
			{
				tab_pix[2*nbpix]=ULONGDATA(pgm_latemap)[i];
				tab_pix[2*nbpix+1]=i;
				nbpix++;
			}

		qsort(tab_pix, nbpix, 2*sizeof(uint32_t), qsort_function_on_uint32);

	}

	/*for(i=0; i<N_pgm; i++)
	{
		if(ULONGDATA(output_pgm_label)[i]==1) ULONGDATA(output_pgm_label)[i]=0;
	}*/

	/*writeimage(output_pgm_label, argv[4]);
	freeimage(output_pgm_label);

	return(0);*/
	//writeimage(output_pgm_label, "debug.pgm");


	list_current_pixels=list_newlist(MODE_FIFO,1000);
	list_next_pixels=list_newlist(MODE_FIFO,1000);
	if(list_current_pixels==NULL || list_next_pixels==NULL)
	{
		fprintf(stderr, "Error: Error when creating list.\n");
		return(1);
	}

	if(pgm_latemap!=NULL)
	{
		signed_count=nbpix-1;
		todilate=tab_pix[2*(signed_count-1)];
		while(signed_count>=0 && tab_pix[2*signed_count]>=(uint32_t)todilate)
		{
			list_push_uint32_t(list_next_pixels, tab_pix[2*signed_count+1]);
			signed_count=signed_count-1;
		}
	}
	else
	{
		signed_count=-1;
		todilate=0;
		for(i=0; i<N_pgm; i++)
			if(ULONGDATA(output_pgm_label)[i]>1)
			{
				list_push_uint32_t(list_next_pixels, i);
			}
	}

	while(signed_count>=0 || !list_isempty(list_next_pixels))
	{
		tlist=list_next_pixels;
		list_next_pixels=list_current_pixels;
		list_current_pixels=tlist;

		fprintf(stdout, "Go\n");

		while(!list_isempty(list_current_pixels))
		{
			current_pixel = list_pop_uint32_t(list_current_pixels);
			for(i=0; i<6; i++)
			{
				voisin=voisin6(current_pixel, 2*i, rs_pgm, ps_pgm, N_pgm);
				if(voisin!=-1 && ULONGDATA(output_pgm_label)[voisin]==1)
				{
					ULONGDATA(output_pgm_label)[voisin]=ULONGDATA(output_pgm_label)[current_pixel];
					list_push_uint32_t(list_next_pixels, voisin);
				}
			}
		}

		if(todilate>=0) todilate--;

		if(pgm_latemap!=NULL)
		{
			while(signed_count>=0 && tab_pix[2*signed_count]>=(uint32_t)todilate)
			{
				list_push_uint32_t(list_next_pixels, tab_pix[2*signed_count+1]);
				signed_count=signed_count-1;
			}
		}




	}

	list_delete(list_next_pixels, NO_FREE_DATA);
	list_delete(list_current_pixels, NO_FREE_DATA);
	if(pgm_latemap!=NULL)
	{
		free(tab_pix);
		freeimage(pgm_latemap);
	}


	writeimage(output_pgm_label, argv[4]);

	freeimage(output_pgm_label);



	return(0);
}

