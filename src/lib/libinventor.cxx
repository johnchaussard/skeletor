//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

//Librairie perso
#include <libinventor.h>


iv_scene* inventor_new_scene(char *output_file)
{
	iv_scene *result;

	result=(iv_scene*)calloc(1, sizeof(iv_scene));
	if(result==NULL)
	{
		fprintf(stderr, "inventor_new_scene(): Memory allocation error (1).\n");
		return(NULL);
	}
	result->num_tabulations=0;

	result->output=fopen(output_file, "wb");
	if(result->output == NULL)
	{
		fprintf(stderr, "inventor_new_scene(): Could not create file %s.\n", output_file);
		return(NULL);
	}

	fprintf(result->output, "#Inventor V2.1 ascii\n\n");
	fprintf(result->output, "LightModel {\n\tmodel PHONG\n\t}\n");
	fprintf(result->output, "DrawStyle {\n\tpointSize 1.0\n\tlineWidth 1.0\n}\n");
	fprintf(result->output, "ShapeHints {\n\tvertexOrdering COUNTERCLOCKWISE\n}\n");

	return(result);
}


void inventor_delete_scene(iv_scene* scene)
{
	fclose(scene->output);
	free(scene);
}


void inventor_add_material_to_scene(iv_scene* scene, char *name, float ambiantR, float ambiantV, float ambiantB, float diffuseR, float diffuseV, float diffuseB, float emissiveR, float emissiveV, float emissiveB, float specularR, float specularV, float specularB, float shininess, float transparency)
{
	inventor_positioncursor(scene);
	fprintf(scene->output, "DEF %s Material {\n", name);
	scene->num_tabulations++;
	inventor_positioncursor(scene);
	fprintf(scene->output, "ambientColor\t%.3f %.3f %.3f\n", ambiantR, ambiantV, ambiantB);
	inventor_positioncursor(scene);
	fprintf(scene->output, "emissiveColor\t%.3f %.3f %.3f\n", emissiveR, emissiveV, emissiveB);
	inventor_positioncursor(scene);
	fprintf(scene->output, "diffuseColor\t%.3f %.3f %.3f\n", diffuseR, diffuseV, diffuseB);
	inventor_positioncursor(scene);
	fprintf(scene->output, "specularColor\t%.3f %.3f %.3f\n", specularR, specularV, specularB);
	inventor_positioncursor(scene);
	fprintf(scene->output, "shininess\t%.3f\n", shininess);
	inventor_positioncursor(scene);
	fprintf(scene->output, "transparency\t%.3f\n", transparency);
	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");
}


void inventor_set_drawstyle(iv_scene *scene, char visibility, uint32_t linewidth, uint32_t pointsize)
{
	inventor_positioncursor(scene);
	fprintf(scene->output, "DrawStyle {\n");
	inventor_positioncursor(scene);
	if(visibility!=INVISIBLE)
		fprintf(scene->output, "\tstyle FILLED\n");
	else
		fprintf(scene->output, "\tstyle INVISIBLE\n");
	inventor_positioncursor(scene);
	fprintf(scene->output, "\tlineWidth %d.0\n", linewidth);
	inventor_positioncursor(scene);
	fprintf(scene->output, "\tpointSize %d.0\n", pointsize);
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");
}


void inventor_new_object(iv_scene* scene)
{
	inventor_positioncursor(scene);
	fprintf(scene->output, "Separator {\n");
	scene->num_tabulations++;
}


void inventor_close_object(iv_scene* scene)
{
	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");
}


void inventor_positioncursor(iv_scene* scene)
{
	uint32_t i;

	if(scene->num_tabulations < 0) fprintf(stderr, "inventor_positioncursor(): Warning, an object was closed and never opened. Please, check code.\n");

	for(i=0; i<(uint32_t)scene->num_tabulations; i++)
	{
		fprintf(scene->output, "\t");
	}
}


void inventor_call_defined(iv_scene* scene, char *name)
{
	inventor_positioncursor(scene);
	fprintf(scene->output, "USE %s\n", name);
}







/**************************************************
     INVENTOR POINTS
**************************************************/

//The given point arrays should be sorted if it is to be used for edges or faces
void inventor_declare_points(iv_scene* scene, char *name, uint32_t* tab_pt, uint32_t num_pt, uint32_t rs, uint32_t ps, char amira_patch)
{
	uint32_t i;

	if(scene->open_face>0) fprintf(stderr, "inventor_declare_points(): Warning, declaring vertices inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "inventor_declare_points(): Warning, declaring vertices inside a set of edges.\n");

	if(num_pt>0)
	{
		inventor_positioncursor(scene);
		if(name!=NULL)
		{
			fprintf(scene->output, "DEF %s ", name);
		}
		fprintf(scene->output, "Coordinate3 {\n");
		scene->num_tabulations++;
		inventor_positioncursor(scene);
		fprintf(scene->output, "point [\n");

		for(i=0; i<num_pt; i++)
		{
			inventor_positioncursor(scene);
			if(amira_patch==0)
				fprintf(scene->output, "%d %d %d,\n", (tab_pt[i]%rs), ((tab_pt[i]%ps)/rs), (tab_pt[i]/ps));  //z - x - y
			else
				fprintf(scene->output, "%.1f %.1f %.1f,\n", (float)((tab_pt[i]%rs)-0.5), (float)(((tab_pt[i]%ps)/rs)-0.5), (float)((tab_pt[i]/ps)-0.5));
		}

		inventor_positioncursor(scene);
		fprintf(scene->output, "]\n");
		scene->num_tabulations--;
		inventor_positioncursor(scene);
		fprintf(scene->output, "}\n");
	}
}


void inventor_use_previously_defined_points(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "inventor_use_previously_defined_points(): Warning, recalling vertices inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "inventor_use_previously_defined_points(): Warning, recalling vertices inside a set of edges.\n");

	inventor_positioncursor(scene);
	fprintf(scene->output, "PointSet {\n");
	inventor_positioncursor(scene);
	fprintf(scene->output, "\tstartIndex 0\n");
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");
}





/**************************************************
     INVENTOR EDGES
**************************************************/


void inventor_init_edgeset(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "inventor_init_edgeset(): Warning, initializing a set of edges inside a set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "inventor_init_edgeset(): Warning, initializing a set of edges inside another set of edges.\n");

	//Prepare for a new list of edges to draw
	inventor_positioncursor(scene);
	fprintf(scene->output, "IndexedLineSet {\n");
	scene->num_tabulations++;
	inventor_positioncursor(scene);
	fprintf(scene->output, "coordIndex [\n");
	inventor_positioncursor(scene);

	scene->open_edge++;
}


void inventor_close_edgeset(iv_scene* scene)
{
	if(scene->open_edge<=0) fprintf(stderr, "inventor_close_edgeset(): Warning, closing a set of edges but no set of edges was previously opened.\n");

	fprintf(scene->output, "\n");
	inventor_positioncursor(scene);
	fprintf(scene->output, "]\n");
	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");

	scene->open_edge--;
}


//The given point array must be sorted
int32_t inventor_draw_curve(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_edge, uint32_t num_pt_edge)
{
	uint32_t i, t;

	if(scene->open_face!=0) fprintf(stderr, "inventor_draw_curve(): Warning, drawing edges inside a set of faces.\n");
	if(scene->open_edge<=0) fprintf(stderr, "inventor_draw_curve(): Warning, drawing edges but no set of edges were initialized.\n");

	for(i=0; i<num_pt_edge; i++)
	{
		t=dichotomic_research_on_uint32t(tab_pt, tab_pt_edge[i], num_pt);
		if(t==0)
		{
			fprintf(stderr, "inventor_draw_curve(): Could not find vertice belonging to curve\n");
			return(-1);
		}
		fprintf(scene->output, "%d, ", t-1);
	}
	fprintf(scene->output, "-1, ");
	return(0);
}


//The given point array must be sorted
int32_t inventor_draw_cubicalcomplex_edges(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_edges, uint32_t num_edges, char type_edge, uint32_t rs, uint32_t ps)
{
	uint32_t i, nxt_pt, tab[2];

	if(scene->open_face!=0) fprintf(stderr, "inventor_draw_cubicalcomplex_edges(): Warning, drawing edges inside a set of faces.\n");
	if(scene->open_edge<=0) fprintf(stderr, "inventor_draw_cubicalcomplex_edges(): Warning, drawing edges but no set of edges was initialized.\n");

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
				fprintf(stderr, "inventor_draw_edges(): unknown edge type given in parameter.\n");
				return(-1);
		}

		//For each edge, find its two vertices in the tab_pt array and write the output file
		for(i=0; i<num_edges; i++)
		{
			tab[0]=tab_edges[i];
			tab[1]=tab_edges[i]+nxt_pt;

			if(inventor_draw_curve(scene, tab_pt, num_pt, tab, 2)<0)
			{
				fprintf(stderr, "inventor_draw_basic_edge(): Error in subfunction.\n");
				return(-1);
			}
		}

	}
	return(0);
}



/**************************************************
     INVENTOR FACES
**************************************************/


void inventor_init_faceset(iv_scene* scene)
{
	if(scene->open_face>0) fprintf(stderr, "inventor_init_faceset(): Warning, initializing a set of faces inside another set of faces.\n");
	if(scene->open_edge>0) fprintf(stderr, "inventor_init_faceset(): Warning, initializing a set of faces inside a set of edges.\n");

	//Prepare for a new list of faces to draw
	inventor_positioncursor(scene);
	fprintf(scene->output, "IndexedFaceSet {\n");
	scene->num_tabulations++;
	inventor_positioncursor(scene);
	fprintf(scene->output, "coordIndex [\n");
	inventor_positioncursor(scene);

	scene->open_face++;
}


void inventor_close_faceset(iv_scene* scene)
{
	if(scene->open_face<=0) fprintf(stderr, "inventor_close_faceset(): Warning, closing a set of faces but no set of faces was previously opened.\n");

	fprintf(scene->output, "\n");
	inventor_positioncursor(scene);
	fprintf(scene->output, "]\n");
	scene->num_tabulations--;
	inventor_positioncursor(scene);
	fprintf(scene->output, "}\n");

	scene->open_face--;
}


int32_t inventor_draw_surface(iv_scene* scene, uint32_t *tab_pt, uint32_t num_pt, uint32_t *tab_pt_face, uint32_t num_pt_face)
{
	uint32_t i, t, start, have_to_close;

	if(scene->open_face<=0) fprintf(stderr, "inventor_draw_surface(): Warning, drawing faces but no set of faces was initialized.\n");
	if(scene->open_edge!=0) fprintf(stderr, "inventor_draw_surface(): Warning, drawing faces inside a set of edges.\n");

	start=0;
	have_to_close=0;
	for(i=0; i<num_pt_face; i++)
	{
		t=dichotomic_research_on_uint32t(tab_pt, tab_pt_face[i], num_pt);
		if(i==0)
			start=t;
		else if(i==num_pt_face-1 && t!=start) //Last point to draw is not closing the face
			have_to_close=1;
		if(t==0)
		{
			fprintf(stderr, "inventor_draw_surface(): Could not find vertice belonging to face\n");
			return(-1);
		}
		fprintf(scene->output, "%d, ", t-1);
	}
	if(have_to_close==1)
		fprintf(scene->output, "%d, ", start-1);
	fprintf(scene->output, "-1, ");
	return(0);
}


//The given point array must be sorted
int32_t inventor_draw_cubicalcomplex_faces(iv_scene* scene, uint32_t* tab_pt, uint32_t num_pt, uint32_t* tab_faces, uint32_t num_faces, char type_face, uint32_t rs, uint32_t ps, int32_t shift)
{
	uint32_t i, tab[4], nxt_pt1, nxt_pt2;

	if(scene->open_face<=0) fprintf(stderr, "inventor_draw_cubicalcomplex_faces(): Warning, drawing faces but no set of faces was initialized.\n");
	if(scene->open_edge!=0) fprintf(stderr, "inventor_draw_cubicalcomplex_faces(): Warning, drawing faces inside a set of edges.\n");


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
				fprintf(stderr, "inventor_draw_faces(): unknown face type given in parameter.\n");
				return(-1);
		}

		//For each face, find its four vertices in the tab_pt array and write the output file
		for(i=0; i<num_faces; i++)
		{
			tab[0]=tab_faces[i]+shift;
			tab[1]=tab_faces[i]+nxt_pt1+shift;
			tab[2]=tab_faces[i]+nxt_pt1+nxt_pt2+shift;
			tab[3]=tab_faces[i]+nxt_pt2+shift;

			if(inventor_draw_surface(scene, tab_pt, num_pt, tab, 4) <0)
			{
				fprintf(stderr, "inventor_draw_basic_faces(): Error in subfunction.\n");
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



