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

#define USAGE "<pgm_volume> <cca_skel> <cca_skel_main_faces> <pgm_label_output_image>"


int32_t main(int argc, char *argv[])
{
	struct xvimage *cca_skel_main_faces, *cca_skel, *output_pgm_label, *pgm_volume;
	list *decomposition;
	complexe *point, *t;
	uint32_t i, rs_pgm, cs_pgm, d_pgm, rs_cca, cs_cca, d_cca, N_pgm, N_cca, ps_cca, ps_pgm, label, pix_cca, pix_pgm, x, y, z, nbpix;


	//uint32_t turn=0;


	//*******************************************
	//Open skeleton image
	//*******************************************
	if (argc!=5)
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

	for(i=0; i<N_pgm; i++)
		if(ULONGDATA(output_pgm_label)[i] == 1)
			ULONGDATA(output_pgm_label)[i]=0;





	writeimage(output_pgm_label, argv[4]);

	freeimage(output_pgm_label);



	return(0);
}


