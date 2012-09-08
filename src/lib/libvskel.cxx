//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

//Librairie perso
#include <libvskel.h>



iv_scene* vskel_new_scene(char *output_file, double tx, double ty, double tz, double scale)
{
	iv_scene *result;

	result=(iv_scene*)calloc(1, sizeof(iv_scene));
	if(result==NULL)
	{
		fprintf(stderr, "vskel_new_scene(): Memory allocation error (1).\n");
		return(NULL);
	}
	result->num_tabulations=0;

	result->output=fopen(output_file, "wb");
	if(result->output == NULL)
	{
		fprintf(stderr, "vskel_new_scene(): Could not create file %s.\n", output_file);
		return(NULL);
	}

	fprintf(result->output, "#SkelFormat\n");
	fprintf(result->output, "#%f %f %f %f\n", -tx, -ty, -tz, 1.0/scale);
	fprintf(result->output, "#Transformation from file coordinates to grid coordinates:\n");
	fprintf(result->output, "#  (x_file + %f) * %f  ->  z_grid\n", -tx, 1.0/scale);
	fprintf(result->output, "#  (y_file + %f) * %f  ->  x_grid\n", -ty, 1.0/scale);
	fprintf(result->output, "#  (z_file + %f) * %f  ->  y_grid\n", -tz, 1.0/scale);

	return(result);
}


void vskel_delete_scene(iv_scene* scene)
{
	fclose(scene->output);
	free(scene);
}



void vskel_positioncursor(iv_scene* scene)
{
	uint32_t i;

	if(scene->num_tabulations < 0) fprintf(stderr, "vskel_positioncursor(): Warning, an object was closed and never opened. Please, check code.\n");

	for(i=0; i<(uint32_t)scene->num_tabulations; i++)
	{
		fprintf(scene->output, "\t");
	}
}







/**************************************************
     INVENTOR POINTS AND WEIGHTS
**************************************************/

//The given point arrays should be sorted if it is to be used for edges or faces
void vskel_declare_points(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t rs, uint32_t ps, double tx, double ty, double tz, double scale)
{
	uint32_t i;

	if(scene->open_face>0) fprintf(stderr, "vskel_declare_points(): Warning, declaring vertices inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "vskel_declare_points(): Warning, declaring vertices inside a set of edges.\n");

	if(num_pt>0)
	{
		vskel_positioncursor(scene);
		fprintf(scene->output, "point [\n");
		scene->num_tabulations++;

		for(i=0; i<num_pt; i++)
		{
			vskel_positioncursor(scene);
			fprintf(scene->output, "%f %f %f,\n", (tab_pt[i]/ps)*scale +tx, (tab_pt[i]%rs)*scale +ty, ((tab_pt[i]%ps)/rs)*scale +tz);  //z - x - y
		}

		scene->num_tabulations--;
		inventor_positioncursor(scene);
		fprintf(scene->output, "]\n");
	}
}


//The given point arrays should be sorted if it is to be used for edges or faces
void vskel_declare_weights(iv_scene* scene, uint32_t* tab_weight, uint32_t num_pt)
{
	uint32_t i;

	if(scene->open_face>0) fprintf(stderr, "vskel_declare_points(): Warning, declaring weights inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "vskel_declare_points(): Warning, declaring weights inside a set of edges.\n");

	if(num_pt>0)
	{
		vskel_positioncursor(scene);
		fprintf(scene->output, "sizemaxball [\n");
		scene->num_tabulations++;

		for(i=0; i<num_pt; i++)
		{
			vskel_positioncursor(scene);
			fprintf(scene->output, "%lu,\n", tab_weight[i]);
		}

		scene->num_tabulations--;
		inventor_positioncursor(scene);
		fprintf(scene->output, "]\n");
	}
}





/**************************************************
     INVENTOR EDGES
**************************************************/


void vskel_init_edgeset(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "vskel_init_edgeset(): Warning, initializing a set of edges inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "vskel_init_edgeset(): Warning, initializing a set of edges inside another set of edges.\n");

	//Prepare for a new list of edges to draw
	inventor_positioncursor(scene);
	fprintf(scene->output, "line [\n");
	scene->num_tabulations++;

	scene->open_edge++;
}


void vskel_init_simplified_edgeset(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "vskel_init_edgeset(): Warning, initializing a set of edges inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "vskel_init_edgeset(): Warning, initializing a set of edges inside another set of edges.\n");

	//Prepare for a new list of edges to draw
	inventor_positioncursor(scene);
	fprintf(scene->output, "seg_line [\n");
	scene->num_tabulations++;

	scene->open_edge++;
}


void vskel_close_edgeset(iv_scene* scene)
{
	if(scene->open_edge<=0) fprintf(stderr, "vskel_close_edgeset(): Warning, closing a set of edges but no set of edges was previously opened.\n");

	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "]\n");

	scene->open_edge--;
}


//The given point array must be sorted
int32_t vskel_draw_curve(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_edge, uint32_t num_pt_edge)
{
	uint32_t i, t;

	if(scene->open_face!=0) fprintf(stderr, "vskel_draw_curve(): Warning, drawing edges inside a set of faces.\n");
	if(scene->open_edge<=0) fprintf(stderr, "vskel_draw_curve(): Warning, drawing edges but no set of edges were initialized.\n");

	inventor_positioncursor(scene);
	for(i=0; i<num_pt_edge; i++)
	{
		t=dichotomic_research_on_uint32t(tab_pt, tab_pt_edge[i], num_pt);
		if(t==0)
		{
			fprintf(stderr, "vskel_draw_curve(): Could not find vertice belonging to curve\n");
			return(-1);
		}
		fprintf(scene->output, "%d, ", t-1);
	}
	fprintf(scene->output, "-1,\n");
	return(0);
}


//The given point array must be sorted
int32_t vskel_draw_cubicalcomplex_edges(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_edges, uint32_t num_edges, char type_edge, uint32_t rs, uint32_t ps)
{
	uint32_t i, nxt_pt, tab[2];

	if(scene->open_face!=0) fprintf(stderr, "vskel_draw_curve(): Warning, drawing edges inside a set of faces.\n");
	if(scene->open_edge<=0) fprintf(stderr, "vskel_draw_curve(): Warning, drawing edges but no set of edges was initialized.\n");

	//Which kind of edge do we need to draw
	if(num_edges>0)
	{
		switch(type_edge)
		{
			case(CC_AX):
				nxt_pt=1;
				break;

			case(CC_AY):
				nxt_pt=rs;
				break;

			case(CC_AZ):
				nxt_pt=ps;
				break;

			default:
				fprintf(stderr, "vskel_draw_curve(): unknown edge type given in parameter.\n");
				return(-1);
		}

		//For each edge, find its two vertices in the tab_pt array and write the output file
		for(i=0; i<num_edges; i++)
		{
			tab[0]=tab_edges[i];
			tab[1]=tab_edges[i]+nxt_pt;

			if(inventor_draw_curve(scene, tab_pt, num_pt, tab, 2)<0)
			{
				fprintf(stderr, "vskel_draw_curve(): Error in subfunction.\n");
				return(-1);
			}
		}

	}
	return(0);
}



/**************************************************
     INVENTOR FACES
**************************************************/


void vskel_init_faceset(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "vskel_init_faceset(): Warning, initializing a set of faces inside another set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "vskel_init_faceset(): Warning, initializing a set of faces inside a set of edges.\n");

	//Prepare for a new list of faces to draw
	inventor_positioncursor(scene);
	fprintf(scene->output, "faces [\n");
	scene->num_tabulations++;

	scene->open_face++;
}


void vskel_close_faceset(iv_scene* scene)
{
	if(scene->open_face<=0) fprintf(stderr, "vskel_close_faceset(): Warning, closing a set of faces but no set of faces was previously opened.\n");

	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "]\n");

	scene->open_face--;
}


int32_t vskel_draw_surface(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_face, uint32_t num_pt_face)
{
	uint32_t i, t, start, have_to_close;

	if(scene->open_face<=0) fprintf(stderr, "vskel_draw_surface(): Warning, drawing faces but no set of faces was initialized.\n");
	if(scene->open_edge!=0) fprintf(stderr, "vskel_draw_surface(): Warning, drawing faces inside a set of edges.\n");

	start=0;
	have_to_close=0;
	inventor_positioncursor(scene);
	for(i=0; i<num_pt_face; i++)
	{
		t=dichotomic_research_on_uint32t(tab_pt, tab_pt_face[i], num_pt);
		if(i==0)
			start=t;
		else if(i==num_pt_face-1 && t!=start) //Last point to draw is not closing the face
			have_to_close=1;
		if(t==0)
		{
			fprintf(stderr, "vskel_draw_surface(): Could not find vertice belonging to face\n");
			return(-1);
		}
		fprintf(scene->output, "%d, ", t-1);
	}
	if(have_to_close==1)
		fprintf(scene->output, "%d, ", start-1);
	fprintf(scene->output, "-1,\n");
	return(0);
}


//The given point array must be sorted
int32_t vskel_draw_cubicalcomplex_faces(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_faces, uint32_t num_faces, char type_face, uint32_t rs, uint32_t ps)
{
	uint32_t i, tab[4], nxt_pt1, nxt_pt2;

	if(scene->open_face<=0) fprintf(stderr, "vskel_draw_cubicalcomplex_faces(): Warning, drawing faces but no set of faces was initialized.\n");
	if(scene->open_edge!=0) fprintf(stderr, "vskel_draw_cubicalcomplex_faces(): Warning, drawing faces inside a set of edges.\n");


	if(num_faces>0)
	{
		switch(type_face)
		{
			case(CC_FXY):
				nxt_pt1=1;
				nxt_pt2=rs;
				break;

			case(CC_FXZ):
				nxt_pt1=1;
				nxt_pt2=ps;
				break;

			case(CC_FYZ):
				nxt_pt1=rs;
				nxt_pt2=ps;
				break;

			default:
				fprintf(stderr, "vskel_draw_cubicalcomplex_faces(): unknown face type given in parameter.\n");
				return(-1);
		}

		//For each face, find its four vertices in the tab_pt array and write the output file
		for(i=0; i<num_faces; i++)
		{
			tab[0]=tab_faces[i];
			tab[1]=tab_faces[i]+nxt_pt1;
			tab[2]=tab_faces[i]+nxt_pt1+nxt_pt2;
			tab[3]=tab_faces[i]+nxt_pt2;

			if(inventor_draw_surface(scene, tab_pt, num_pt, tab, 4) <0)
			{
				fprintf(stderr, "vskel_draw_cubicalcomplex_faces(): Error in subfunction.\n");
				return(-1);
			}

			/*tab[3]=tab_faces[i]+nxt_pt1+shift;
			tab[1]=tab_faces[i]+nxt_pt2+shift;

			if(inventor_draw_surface(scene, tab_pt, num_pt, tab, 4) <0)
			{
				fprintf(stderr, "inventor_draw_basic_faces(): Error in subfunction.\n");
				return(-1);
			}*/
		}

	}

	return(0);
}


