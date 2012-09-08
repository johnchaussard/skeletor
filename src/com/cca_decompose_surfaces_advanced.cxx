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

#define USAGE "<cca_image> <intersect_edges_cca_image> <inventor_output_file_no_extension> <split|fusion> <filter.cc|NULL> <keep|reject> <output_surfaces.cc|NULL> [-amira]"

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


//Function allows to cut a specific part of the image and only extract the surfaces having one element in a given zone
uint32_t is_in_zone(uint32_t pixnum, uint32_t rs, uint32_t ps, uint32_t bordinfx, uint32_t bordsupx, uint32_t bordinfy, uint32_t bordsupy, uint32_t bordinfz, uint32_t bordsupz)
{
	uint32_t x, y, z;

	x=getxfrompixnum(pixnum, rs, ps);
	y=getyfrompixnum(pixnum, rs, ps);
	z=getzfrompixnum(pixnum, rs, ps);

	return( (x>=bordinfx && x<=bordsupx) && (y>=bordinfy && y<=bordsupy) && (z>=bordinfz && z<=bordsupz) );
}

//Will label all the cubes which are not belonging to cca_image
/*struct xvimage *cca_label_cube_components(struct xvimage *cca_image)
{
	list *elements, *temp;
	uint32_t rs, ps, N, cs, d, i, j, l;
	face_desc g, *e, *h;
	unsigned char f;
	struct xvimage *label;

	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	ps=cs*rs;
	d=depth(cca_image);
	N=d*ps;

	label=allocimage(NULL, rs, cs, d, VFF_TYP_4_BYTE);
	if(label==NULL)
	{
		fprintf(stderr, "cca_label_cube_components(): Memory allocation error(2).\n");
		return(NULL);
	}

	elements=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc));
	if(elements==NULL)
	{
		fprintf(stderr, "cca_label_cube_components(): Memory allocation error(1).\n");
		return(NULL);
	}

	l=1;
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_VOL) == 0)
		{
			g.pixnumber=i;
			g.face_type=CC_VOL;
			list_push(elements, &g);
			LONGDATA(label)[i]=l;
		}

		//A new cube is found, let's develop its neighbours
		while(!list_isempty(elements))
		{
			e=list_pop(elements);

		temp=cca_list_starface_dimension_direct_over(e->pixnumber, e->face_type, rs, cs, d);
		while(!list_isempty(temp))
		{
			h=list_pop(temp);
			if(	((UCHARDATA(image)[h->pixnumber] & h->face_type) == 0)  &&
				((UCHARDATA(container)[h->pixnumber] & h->face_type) != 0) &&
				((avoid==NULL) || ((UCHARDATA(avoid)[h->pixnumber] & h->face_type) == 0)) )
			{
				g.pixnumber=h->pixnumber;
				g.face_type=h->face_type;
				list_push(elements, &g);
				UCHARDATA(image)[h->pixnumber] |= h->face_type;
			}
		}

		list_delete(temp, NO_FREE_DATA);

		temp=cca_list_subfaces_dimension_direct_under(e->pixnumber, e->face_type, rs, ps);
		while(!list_isempty(temp))
		{
			h=list_pop(temp);
			if(	((UCHARDATA(image)[h->pixnumber] & h->face_type) == 0)  &&
				((UCHARDATA(container)[h->pixnumber] & h->face_type) != 0) &&
				((avoid==NULL) || ((UCHARDATA(avoid)[h->pixnumber] & h->face_type) == 0)) )
			{
				g.pixnumber=h->pixnumber;
				g.face_type=h->face_type;
				list_push(elements, &g);
				UCHARDATA(image)[h->pixnumber] |= h->face_type;
			}
		}

		list_delete(temp, NO_FREE_DATA);
	}

	list_delete(elements, NO_FREE_DATA);

	return(0);
*/


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *edges, *cca_image, *filter;
	FILE *amira_script=NULL;
	char mode_amira, mode, strategy;
	double r, v, b;
	iv_scene* scene=NULL;
	list *ss_result;
	complexe *temp, *intersect_edges, *point;
	uint32_t i, rs, ps, keep, k, max, num_pt, *tab_pt, kept;
	char name[BASE_ALLOC];
	long ltime;

	/*uint32_t surf[10000][2];
	struct xvimage *label;
	list *l;
	unsigned char f;
	face_desc *g;*/

	//label=readimage("label.pgm");

	ltime=time(NULL);
	srand(ltime);



	//*******************************************
	//Checking input values
	//*******************************************
	if (argc<8 || argc>9)
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

	edges=readimage(argv[2]);
	if (edges==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(-1);
	}
	else if(datatype(edges)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}

	if(strcmp(argv[5],"NULL")!=0)
	{
		filter=readimage(argv[5]);
		if(filter==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[5]);
			return(-1);
		}
		else if(datatype(filter)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error: only CC image supported\n");
			return(-1);
		}
	}
	else
		filter=NULL;

	if(strcmp(argv[6], "keep")==0)
	{
		strategy=KEEP;
	}
	else if(strcmp(argv[6], "reject")==0)
	{
		strategy=REJECT;
	}
	else
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}


	mode_amira=0;
	if(argc==9)
	{
		if(strcmp(argv[8], "-amira")!=0)
		{
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			freeimage(image);
			freeimage(edges);
			return(-1);
		}
		else
		{
			mode_amira=1;
		}
	}

	if(strcmp(argv[4], "fusion")==0)
	{
		mode=FUSION;
	}
	else if(strcmp(argv[4], "split")==0)
	{
		mode=SPLIT;
	}
	else
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		freeimage(image);
		freeimage(edges);
		return(-1);
	}


	rs=rowsize(image);
	ps=colsize(image)*rs;

	if(mode==SPLIT)
	{
		//Each surface will be written in different Inventor File
		//Create an AVIZO script which will allow to open all of them
		sprintf(name, "%s_avizo_load_script.hx", argv[3]);
		amira_script=fopen(name, "wb");
		if(amira_script==NULL)
		{
			fprintf(stderr, "Error: could not create file %s. Check directory permission.\n", name);
			freeimage(image);
			freeimage(edges);
			return(-1);
		}
		amira_script_init(amira_script);
	}
	else
	{
		sprintf(name, "%s.iv", argv[3]);
	}


	if(strcmp(argv[7], "NULL")!=0)
	{
		cca_image=allocimage(NULL, rs, ps/rs, depth(image), VFF_TYP_1_BYTE);
		if(cca_image==NULL)
		{
			fprintf(stderr, "Error: could not allocate memory\n");
			freeimage(image);
			freeimage(edges);
			return(-1);
		}
	}
	else
	{
		cca_image=NULL;
	}




	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	if(mode==FUSION)
	{
		scene=inventor_new_scene(name);
	}

	//********************************************************
	//Make surface segmentation
	//********************************************************

	//Get the intersection edges
	intersect_edges=cca_to_complexe(edges);
	freeimage(edges);
	if(intersect_edges==NULL)
	{
		fprintf(stderr, "Error in function cca_to_complexe()\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return (-1);
	}


	//Make separation of the complex into surface components
	ss_result=cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image(image, intersect_edges, filter, strategy);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image() failed.\n");
		inventor_delete_scene(scene);
		freeimage(image);
		return(-1);
	}
	//We don't need the image anymore
	freeimage(image);

	fprintf(stdout, "Found %d surfaces\n", list_getsize(ss_result) -2);


	//The first item is the set of all vertices...
	temp=(complexe*)list_pop_pointer(ss_result);
	tab_pt=NULL;
	num_pt=0;
	if(mode==SPLIT)
	{
		//We don't care.
		complexe_free_complexe(temp);
		point=NULL;
	}
	else if(mode==FUSION)
	{
		for(i=0; i<ss_result->size; i++)
		{
			//Choose a random color for surfaces
			r=((double)rand())/((double)RAND_MAX);
			v=((double)rand())/((double)RAND_MAX);
			b=((double)rand())/((double)RAND_MAX);
			sprintf(name, "Surf_%d", i);
			inventor_add_material_to_scene(scene, name, r, v, b, r/2.0, v/2.0, b/2.0, r/3.0, v/3.0, b/3.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}

		//All the surfaces will go in the same file.
		//The array of points therefore interest us, we keep it and write it in the inventor file
		point=temp;
		tab_pt=point->tab_pt_obj;
		num_pt=point->num_pt_obj;
		inventor_new_object(scene);
		//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
		inventor_declare_points(scene, (char*)"Points", tab_pt, num_pt, rs, ps, mode_amira);

	}


	//The second item is the set of all intersect edges... We don't care.
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);






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
			if(cca_image!=NULL)
				complexe_add_to_cca(cca_image, temp);

/*			if(temp->num_fxy>0) {k=temp->tab_fxy[0]; f=CC_FXY;}
			else if(temp->num_fxz>0) {k=temp->tab_fxz[0]; f=CC_FXZ;}
			else if(temp->num_fyz>0) {k=temp->tab_fyz[0]; f=CC_FYZ;}
			else assert(0);

			l=cca_list_container(cca_image, k, getxfrompixnum(k, rs, ps), getyfrompixnum(k, rs, ps), getzfrompixnum(k, rs, ps), f, rs, ps);
			assert(l->cpt==2);
			while(!list_isempty(l))
			{
				g=(face_desc*)list_pop(l);
				surf[kept][l->cpt -1]=LONGDATA(labels)[g->pixnumber];
				free(g);
			}
			assert(surf[kept][0] != surf[kept][1]);
			if(surf[kept][0] > surf[kept][1])
			{
				k=surf[kept][1];
				surf[kept][1]=surf[kept][0];
				surf[kept][0]=k;
			}
*/
			kept++;


			if(mode==SPLIT)
			{
				complexe_compute_vertex_array(temp, rs, ps);

				//Create an output file for the surface
				sprintf(name, "%s.surf%d.%d.iv", argv[3], temp->num_fxy+temp->num_fyz+temp->num_fxz, i);
				scene=inventor_new_scene(name);
				if(scene==NULL)
				{
					fprintf(stderr, "Error: could not create new scene for file %s.\n", name);
					while(!list_isempty(ss_result))
					{
						temp=(complexe*)list_pop_pointer(ss_result);
						complexe_free_complexe(temp);
					}
					list_delete(ss_result, NO_FREE_DATA);
					return(-1);
				}


				//Choose a random color for surfaces
				r=((double)rand())/((double)RAND_MAX);
				v=((double)rand())/((double)RAND_MAX);
				b=((double)rand())/((double)RAND_MAX);
				sprintf(name, "Surf_%d", i);
				inventor_add_material_to_scene(scene, name, r, v, b, r/2.0, v/2.0, b/2.0, r/3.0, v/3.0, b/3.0, 0.0, 0.0, 0.0, 0.0, 0.0);


				//And initialise the scene and the points
				inventor_new_object(scene);
				//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
				inventor_declare_points(scene, (char*)"Points", temp->tab_pt_obj, temp->num_pt_obj, rs, ps, mode_amira);

				tab_pt=temp->tab_pt_obj;
				num_pt=temp->num_pt_obj;
				temp->num_pt_obj=0;
				temp->tab_pt_obj=NULL;
			}

			inventor_new_object(scene);

			sprintf(name, "Surf_%d", i);

			//inventor_call_defined(scene, "Points");
			complexe_to_inventor(scene, temp, num_pt, tab_pt, rs, ps, mode_amira, NULL, NULL, name, NULL);

			inventor_close_object(scene);

			fflush(scene->output);

			if(mode==SPLIT)
			{
				inventor_close_object(scene);
				inventor_delete_scene(scene);

				//For the amira script
				k=0;
				max=0;
				while(name[k]!='\0')
				{
					if(name[k]=='/')
						max=k+1;

					k++;
				}

				amira_script_add_iv_file(amira_script, &name[max], 1, 30*(i+1));

				free(tab_pt);
			}



		}
		i++;
		complexe_free_complexe(temp);
	}

	if(mode==FUSION)
	{
		inventor_close_object(scene);
		fclose(scene->output);
	}


	fprintf(stdout, "Kept %d surfaces\n", kept);

	/*for(i=0; i<kept-1; i++)
		for(k=i+1; k<kept; k++)
		{
			if(
*/
	if(cca_image!=NULL)
	{
		writeimage(cca_image, argv[7]);
		freeimage(cca_image);
	}

	//****************************************************
	//Program ends
	//****************************************************
	inventor_delete_scene(scene);
	list_delete(ss_result, NO_FREE_DATA);
	if(filter!=NULL)
		freeimage(filter);

	if(mode==SPLIT)
		fclose(amira_script);


	return(0);
}
