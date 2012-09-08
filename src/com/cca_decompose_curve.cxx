
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

#define USAGE "<cca_image> <inventor_output_file> [-amira]"

#define SPLIT 0
#define FUSION 1



void amira_script_init(FILE *f)
{
	fprintf(f, "# Avizo Script\nremove -all\n\n");
	fprintf(f, "# Create viewers\nviewer setVertical 0\nviewer 0 setAutoRedraw 1\nviewer 0 show\nmainWindow show\n\n");
}


void amira_script_add_iv_file(FILE *f, char *name_file, uint32_t x, uint32_t y)
{
	fprintf(f, "set hideNewModules 0\n");
	fprintf(f, "[ load ${SCRIPTDIR}/%s ] setLabel %s\n", name_file, name_file);
	fprintf(f, "%s setIconPosition %d %d\n", name_file, x, y);
	fprintf(f, "%s fire\n", name_file);
	fprintf(f, "%s fire\n", name_file);
	fprintf(f, "%s setViewerMask 65535\n\n", name_file);
	fprintf(f, "set hideNewModules 0\n");
	fprintf(f, "create HxIvDisplay {Disp_%s}\n", name_file);
	fprintf(f, "Disp_%s setIconPosition %d %d\n", name_file, x+200, y);
	fprintf(f, "Disp_%s data connect %s\n", name_file, name_file);
	fprintf(f, "Disp_%s fire\n", name_file);
	fprintf(f, "Disp_%s drawStyle setIndex 0 0\n", name_file);
	fprintf(f, "Disp_%s fire\n", name_file);
	fprintf(f, "Disp_%s setViewerMask 65535\n", name_file);
	fprintf(f, "Disp_%s setShadowStyle 0\n\n\n", name_file);
	fprintf(f, "viewer 0 redraw\n");
}

void amira_script_close(FILE *f)
{
	fprintf(f, "set hideNewModules 0\n");
	fprintf(f, "viewer 0 setAutoRedraw 1\nviewer 0 redraw\n");
}





int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	char mode_amira;
	double r, v, b;
	iv_scene* scene;
	list *ss_result;
	complexe *cpl, *temp, *point;
	uint32_t i, rs, ps, keep, num_pt, *tab_pt, kept, N;
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
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}


	mode_amira=0;
	if(argc==4)
	{
		if(strcmp(argv[3], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			freeimage(image);
			return(-1);
		}
		else
		{
			mode_amira=1;
		}
	}


	rs=rowsize(image);
	ps=colsize(image)*rs;
	N=depth(image)*ps;



	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);

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

	//Get the intersection edges
	cpl=complexe_new_complexe();
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_PT) != 0)
		{
			if(cca_cardinal_containers(image, i, getxfrompixnum(i, rs, ps), getyfrompixnum(i, rs, ps), getzfrompixnum(i, rs, ps), CC_PT, rs, ps)!=2)
			{
				complexe_add_element(cpl, i, CC_PT);
			}
		}
	}


	//Make separation of the complex into surface components
	ss_result=cca_simple_curve_decomposition(image, cpl);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}
	//We don't need the image anymore
	freeimage(image);

	fprintf(stdout, "Found %d curves\n", list_getsize(ss_result) -1);


	//The first item is the set of all vertices...
	temp=(complexe*)list_pop_pointer(ss_result);
	for(i=0; i<ss_result->size; i++)
	{
		//Choose a random color for surfaces
		r=((double)rand())/((double)RAND_MAX);
		v=((double)rand())/((double)RAND_MAX);
		b=((double)rand())/((double)RAND_MAX);
		sprintf(name, "Curv_%d", i);
		inventor_add_material_to_scene(scene, name, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	//All the surfaces will go in the same file.
	//The array of points therefore interest us, we keep it and write it in the inventor file
	point=temp;
	tab_pt=point->tab_pt_obj;
	num_pt=point->num_pt_obj;
	inventor_new_object(scene);
	//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
	inventor_declare_points(scene, (char*)"Points", tab_pt, num_pt, rs, ps, mode_amira);




	//*******************************************************
	//Send the surfaces to output file
	//*******************************************************

	i=0; //Will be used for the filename
	kept=0;
	while(!list_isempty(ss_result))
	{
		//Get the object to write
		temp=(complexe*)list_pop_pointer(ss_result);


		keep=1;
		if(keep==1)
		{

			kept++;

			inventor_new_object(scene);

			sprintf(name, "Curv_%d", i);
			//inventor_add_material_to_scene(scene, name, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


			//inventor_call_defined(scene, "Points");
			//complexe_to_inventor(scene, temp, num_pt, tab_pt, rs, ps, mode_amira);
			inventor_call_defined(scene, name);
			inventor_init_edgeset(scene);

			inventor_draw_curve(scene, tab_pt, num_pt, temp->tab_pt_obj, temp->num_pt_obj);
			inventor_close_edgeset(scene);

			inventor_close_object(scene);

			fflush(scene->output);




		}
		i++;
		complexe_free_complexe(temp);
	}

	inventor_close_object(scene);



	fprintf(stdout, "Kept %d curves\n", kept);



	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	list_delete(ss_result, NO_FREE_DATA);

	return(0);
}
