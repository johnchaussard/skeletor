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

#define USAGE "<cca_image> <output_file> <vrml|inventor>"

#define IV 0
#define VRML 1


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *edges;

	FILE *output, *amira_script;
	char output_format;
	double r, v, b;
	iv_scene* scene;
	list *ss_result, *ss_result2;
	complexe *temp, *intersect_edges, *point;
	uint32_t i, rs, ps, d, N, lim, erode, j, keep, k, max, num_pt, *tab_pt, kept;
	char name[BASE_ALLOC];


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
		fprintf(stderr, "Error: only CC image supported\n");
		return(-1);
	}


	//We extract some info
	rs=rowsize(image1);
	cs=colsize(image1);
	d=depth(image1);
	N=rs*cs*d;


	//We produce the edges image
	edges=copyimage(image);
	if(edges==NULL)
	{
		fprintf(stderr, "Memory allocation error : not enough memory.\n");
		return(-1);
	}
	//And keep only intersection edges in this image
	pix=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if( (UCHARDATA(image1)[pix] & CC_AX) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AX, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AX);
				}

				if( (UCHARDATA(image1)[pix] & CC_AY) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AY, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AY);
				}

				if( (UCHARDATA(image1)[pix] & CC_AZ) != 0)
				{
					t=cca_cardinal_containers(image1, pix, i, j, k, CC_AZ, rs, rs*cs);
					if(t<3)
						UCHARDATA(image1)[pix]&=(255-CC_AZ);
				}

				pix++;
			}

	for(i=0; i<N; i++)
		UCHARDATA(image1)[i]&=255-(CC_VOL | CC_FXY | CC_FYZ | CC_FXZ | CC_PT);

	cca_makecomplex(image1);



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
		output = fopen(name, "wb");
		if(output==NULL)
		{
			fprintf(stderr, "Error: could not create output file %s (directory exists?).\n", name);
			freeimage(image);
			freeimage(edges);
			return(-1);
		}
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

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);

	scene=inventor_new_scene(10, NULL);

	//Initialize the scene...
	if(scene==NULL)
	{
		fprintf(stderr, "Error when creating new scene.\n");
		return(-1);
	}

	//We add to our object the main materials (keep the surfaces for later)
	inventor_add_material_to_scene(scene, "MatPoint", POS_PT, 0.0, 0.0, 0.0, 0.1, 0.4, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAX", POS_AX, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAY", POS_AY, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAZ", POS_AZ, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVXY", POS_VXY, 0.0, 0.0, 0.0, 0.65, 0.65, 0.65, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVXZ", POS_VXZ, 0.0, 0.0, 0.0, 0.50, 0.50, 0.50, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVYZ", POS_VYZ, 0.0, 0.0, 0.0, 0.80, 0.80, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


	if(mode==FUSION)
	{
		scene->output=output;
		inventor_init_file(scene);
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

	fprintf(stdout, "Found %d surfaces\n", ss_result->cpt -2);


	//The first item is the set of all vertices...
	temp=(complexe*)list_pop(ss_result);
	if(mode==SPLIT)
	{
		//We don't care.
		complexe_free_complexe(temp);
		point=NULL;
		tab_pt=NULL;
		num_pt=0;
	}
	else if(mode==FUSION)
	{
		for(i=0; i<ss_result->cpt; i++)
		{
			//Choose a random color for surfaces
			r=((double)rand())/((double)RAND_MAX);
			v=((double)rand())/((double)RAND_MAX);
			b=((double)rand())/((double)RAND_MAX);
			sprintf(name, "SurfXY_%d", i);
			inventor_add_material_to_scene(scene, name, POS_FXY, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			r+=0.05; v+=0.05; b+=0.05;
			if(r>1.0) r=1.0;
			if(b>1.0) b=1.0;
			if(v>1.0) v=1.0;
			sprintf(name, "SurfXZ_%d", i);
			inventor_add_material_to_scene(scene, name, POS_FXZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			r+=0.05; v+=0.05; b+=0.05;
			if(r>1.0) r=1.0;
			if(b>1.0) b=1.0;
			if(v>1.0) v=1.0;
			sprintf(name, "SurfYZ_%d", i);
			inventor_add_material_to_scene(scene, name, POS_FYZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			inventor_write_material_to_file(scene, POS_FXY);
			inventor_write_material_to_file(scene, POS_FXZ);
			inventor_write_material_to_file(scene, POS_FYZ);
		}

		//All the surfaces will go in the same file.
		//The array of points therefore interest us, we keep it and write it in the inventor file
		point=temp;
		tab_pt=point->tab_pt_obj;
		num_pt=point->num_pt_obj;
		inventor_new_object(scene);
		//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
		inventor_declare_points(scene, "Points", tab_pt, num_pt, rs, ps, mode_amira);

	}


	//The second item is the set of all intersect edges... We don't care.
	temp=(complexe*)list_pop(ss_result);
	complexe_free_complexe(temp);






	//*******************************************************
	//Send the surfaces to output file
	//*******************************************************

	i=0; //Will be used for the filename
	kept=0;
	while(!list_isempty(ss_result))
	{
		//Get the object to write
		temp=(complexe*)list_pop(ss_result);

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
				complexe_compute_vertex_array(temp, rs, ps);

			if(mode==SPLIT)
			{
				//Choose a random color for surfaces
				r=((double)rand())/((double)RAND_MAX);
				v=((double)rand())/((double)RAND_MAX);
				b=((double)rand())/((double)RAND_MAX);
				sprintf(name, "SurfXY_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FXY, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				r+=0.05; v+=0.05; b+=0.05;
				if(r>1.0) r=1.0;
				if(b>1.0) b=1.0;
				if(v>1.0) v=1.0;
				sprintf(name, "SurfXZ_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FXZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				r+=0.05; v+=0.05; b+=0.05;
				if(r>1.0) r=1.0;
				if(b>1.0) b=1.0;
				if(v>1.0) v=1.0;
				sprintf(name, "SurfYZ_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FYZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


				//Create an output file for the surface

				sprintf(name, "%s.surf%d.%d.iv", argv[3], temp->num_fxy+temp->num_fyz+temp->num_fxz, i);
				output = fopen(name, "wb");
				if(output==NULL)
				{
					fprintf(stderr, "Error: could not create output file %s (directory exists?).\n", name);
					while(!list_isempty(ss_result))
					{
						temp=(complexe*)list_pop(ss_result);
						complexe_free_complexe(temp);
					}
					list_delete(ss_result, NO_FREE_DATA);
					inventor_delete_scene(scene);
					return(-1);
				}

				//Link the output with the scene
				scene->output=output;

				//And initialise the scene and the points
				inventor_init_file(scene);
				inventor_new_object(scene);
				//inventor_set_drawstyle(scene, INVISIBLE, 1, 1);
				inventor_declare_points(scene, "Points", temp->tab_pt_obj, temp->num_pt_obj, rs, ps, mode_amira);

				tab_pt=temp->tab_pt_obj;
				num_pt=temp->num_pt_obj;
				temp->num_pt_obj=0;
				temp->tab_pt_obj=NULL;
			}

			inventor_new_object(scene);


			if(mode==FUSION)
			{
				sprintf(name, "SurfXY_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FXY, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				sprintf(name, "SurfXZ_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FXZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				sprintf(name, "SurfYZ_%d", i);
				inventor_add_material_to_scene(scene, name, POS_FYZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			}

			//inventor_call_defined(scene, "Points");
			complexe_to_inventor(scene, temp, num_pt, tab_pt, rs, ps, mode_amira);

			inventor_close_object(scene);

			fflush(scene->output);

			if(mode==SPLIT)
			{
				inventor_close_object(scene);
				fclose(scene->output);

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
