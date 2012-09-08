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

#define USAGE "<pgm_label_image> <inventor_output_file> [-amira]"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	char mode_amira;
	double r, v, b;
	iv_scene* scene;
	list *list_complexe;
	complexe *cpl;
	uint32_t pix, i, rs_pgm, cs_pgm, d_pgm, ps_pgm, N_pgm, rs_cca, cs_cca, d_cca, ps_cca, x, y, z, nbpix, k, *tab_pix, *tab_pt, num_pt;
	int32_t voisin;
	char name[BASE_ALLOC];
	long ltime;

	ltime=time(NULL);
	srand(ltime);



	//*******************************************
	//Checking input values
	//*******************************************
	if (argc<3 || argc>4)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(image)!=VFF_TYP_4_BYTE)
	{
		fprintf(stderr, "Error: only integer PGM image supported\n");
		return(1);
	}

	rs_pgm=rowsize(image);
	cs_pgm=colsize(image);
	d_pgm=depth(image);
	ps_pgm=rs_pgm*cs_pgm;
	N_pgm=ps_pgm*d_pgm;

	mode_amira=0;
	if(argc==4)
	{
		if(strcmp(argv[3], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			return(1);
		}
		else
		{
			mode_amira=1;
		}
	}



	//First, list all voxels of the image which belong to the border of the object, and register their label
	nbpix=0;
	for(i=0; i<N_pgm; i++)
	{
		if(ULONGDATA(image)[i]>0)
		{
			for(k=0; k<6; k++)
			{
				voisin=voisin6(i, 2*k, rs_pgm, ps_pgm, N_pgm);
				if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[i])
				{
					nbpix++;
					break;
				}
			}
		}
	}

	tab_pix=(uint32_t*)calloc(nbpix, 2*sizeof(uint32_t));
	if(tab_pix==NULL)
	{
		fprintf(stderr, "Memory allocation error.\n");
		return(1);
	}

	nbpix=0;
	for(i=0; i<N_pgm; i++)
	{
		if(ULONGDATA(image)[i]>0)
		{
			for(k=0; k<6; k++)
			{
				voisin=voisin6(i, 2*k, rs_pgm, ps_pgm, N_pgm);
				if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[i])
				{
					tab_pix[2*nbpix]=ULONGDATA(image)[i];
					tab_pix[2*nbpix+1]=i;
					nbpix++;
					break;
				}
			}
		}
	}

	qsort(tab_pix, nbpix, 2*sizeof(uint32_t), qsort_function_on_uint32);





	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	scene=inventor_new_scene(argv[2]);

	//Initialize the scene...
	if(scene==NULL)
	{
		fprintf(stderr, "Error when creating new scene.\n");
		return(-1);
	}




	//********************************************************
	//Make surface segmentation
	//********************************************************


	list_complexe=list_newlist(MODE_FIFO, 64);


	cpl=complexe_new_complexe();

	rs_cca=rs_pgm+1;
	cs_cca=cs_pgm+1;
	d_cca=d_pgm+1;
	ps_cca=rs_cca*cs_cca;

	for(i=0; i<nbpix; i++)
	{
		pix=tab_pix[2*i+1];
		x=getxfrompixnum(pix, rs_pgm, ps_pgm);
		y=getyfrompixnum(pix, rs_pgm, ps_pgm);
		z=getzfrompixnum(pix, rs_pgm, ps_pgm);


		voisin=voisin6(pix, EST, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+1+y*rs_cca+z*ps_cca, CC_FYZ);

		voisin=voisin6(pix, OUEST, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+y*rs_cca+z*ps_cca, CC_FYZ);

		voisin=voisin6(pix, SUD, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+(y+1)*rs_cca+z*ps_cca, CC_FXZ);

		voisin=voisin6(pix, NORD, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+y*rs_cca+z*ps_cca, CC_FXZ);

		voisin=voisin6(pix, DERRIERE, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+y*rs_cca+(z+1)*ps_cca, CC_FXY);

		voisin=voisin6(pix, DEVANT, rs_pgm, ps_pgm, N_pgm);
		if(voisin==-1 || ULONGDATA(image)[voisin]!=ULONGDATA(image)[pix])
			complexe_add_element(cpl, x+y*rs_cca+z*ps_cca, CC_FXY);


		if(i==nbpix-1 || tab_pix[2*i]!=tab_pix[2*(i+1)])
		{
			list_push_pointer(list_complexe, cpl);
			if(i!=nbpix-1) cpl=complexe_new_complexe();
		}

	}

	freeimage(image); //Todo : free earlier !




	fprintf(stdout, "Found %d parts.\n", list_getsize(list_complexe));

	for(i=0; i<list_complexe->size; i++)
	{
		//Choose a random color for surfaces
		r=((double)rand())/((double)RAND_MAX);
		v=((double)rand())/((double)RAND_MAX);
		b=((double)rand())/((double)RAND_MAX);
		sprintf(name, "Part_%d", i);
		inventor_add_material_to_scene(scene, name, r, v, b, r/2.0, v/2.0, b/2.0, r/3.0, v/3.0, b/3.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}



	//*******************************************************
	//Send the surfaces to output file
	//*******************************************************

	i=0; //Will be used for the material in the scene
	while(!list_isempty(list_complexe))
	{
		//Get the object to write
		cpl=(complexe*)list_pop_pointer(list_complexe);
		complexe_compute_vertex_array(cpl, rs_cca, ps_cca);

		inventor_new_object(scene);

		tab_pt=cpl->tab_pt_obj;
		num_pt=cpl->num_pt_obj;
		cpl->tab_pt_obj=NULL;
		cpl->num_pt_obj=0;
		inventor_declare_points(scene, (char*)"Pts", tab_pt, num_pt, rs_cca, ps_cca, mode_amira);

		sprintf(name, "Part_%d", i);
		complexe_to_inventor(scene, cpl, num_pt, tab_pt, rs_cca, ps_cca, mode_amira, NULL, NULL, name, name);

		cpl->tab_pt_obj=tab_pt;
		cpl->num_pt_obj=num_pt;

		inventor_close_object(scene);

		fflush(scene->output);

		complexe_free_complexe(cpl);

		i++;
	}



	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	list_delete(list_complexe, NO_FREE_DATA);


	return(0);
}
