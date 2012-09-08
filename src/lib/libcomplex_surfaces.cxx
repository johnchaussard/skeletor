//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Librairie perso
#include <libcomplex.h>
#include <libinventor.h>
#include <liblist.h>




//lst_surfaces must only be made of surfaces
//lst_edges must be made of egdes
//The program scans the surfaces and compute their border (should be in lst_edges)
//New surfaces built from the border of the old ones are then put in the output list
//The list contains first the list linking surfaces and edges (for each  surfaces, a list of index of complexe representing edges of surface),
//then the simplified edges, then simplified surfaces
list *cca_simplify_surfaces_wth_border(list *lst_surfaces, list *lst_edges, uint32_t simplification_factor, uint32_t rs, uint32_t ps, uint32_t N)
{
	list **link_surfaces_wth_curves, *result, *t;
	complexe **tab_edges, *s;
	uint32_t i, j, k;

	//First, we compute for each surface which edge from lst_edge it contains
	link_surfaces_wth_curves=cca_link_surfaces_to_border_edges(lst_surfaces, lst_edges, rs, ps, N);
	if(link_surfaces_wth_curves==NULL)
	{
		fprintf(stderr, "cca_surfaces_to_gc(): cca_link_surfaces_to_border_edges() failed.\n");
		return(NULL);
	}

	result=list_newlist(MODE_FIFO, 64);
	if(result==NULL)
	{
		fprintf(stderr, "cca_surfaces_to_gc(): could not create output list.\n");
		return(NULL);
	}

	list_push_pointer(result, link_surfaces_wth_curves);

	//Build an array for fast access to edges
	tab_edges=(complexe**)calloc(list_getsize(lst_edges), sizeof(complexe*));
	if(tab_edges==NULL)
	{
		fprintf(stderr, "cca_surfaces_to_gc(): memory allocation failed.\n");
		return(NULL);
	}
	for(i=0; i<lst_edges->size; i++)
	{
		s=(complexe*)list_pop_pointer(lst_edges);
		complexe_compute_vertex_array(s, rs, ps);
		tab_edges[i]=cc_to_gc_only_simple_edges(s, simplification_factor, rs, ps, N);
		list_push_pointer(lst_edges, s);
		list_push_pointer(result, tab_edges[i]);
	}



	t=list_newlist(MODE_FIFO, 64);
	//Transform link_surfaces_wth_curve into a list of complex
	for(i=0; i<lst_surfaces->size; i++)
	{
		for (j=0; j<link_surfaces_wth_curves[i]->size; j++)
		{
			k=list_pop_uint32_t(link_surfaces_wth_curves[i]);
			list_push_uint32_t(link_surfaces_wth_curves[i], k);
			list_push_pointer(t, tab_edges[k]);
		}

		//Then, compute the simplified surface from these edges
		s=cc_to_gc_one_surface(t, rs, ps, N);
		list_make_empty(t, NO_FREE_DATA);
		list_push_pointer(result, s);

	}
	list_delete(t, NO_FREE_DATA);

	free(tab_edges);

	return(result);
}



//All edges with cardinal sup or equal to 3 must have been removed from image
//All edges with cardinal sup or equal to 3 must have been removed from image
int32_t cca_get_simple_surface(struct xvimage *cca_image, complexe* result, uint32_t one_face, unsigned char typ_face)
{
	list *l, *container;
	face_desc *data;
	uint32_t rs, ps, x, y, z, px;
	unsigned char face;

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;

	l=list_newlist(MODE_FIFO, 1024);
	if(l==NULL)
	{
		fprintf(stderr, "cca_get_simple_surface(): Error, could not create new list.\n");
		return(-1);
	}

	data=(face_desc*)calloc(1, sizeof(face_desc));
	if(data==NULL)
	{
		fprintf(stderr, "cca_get_simple_surface(): data memory allocation error.\n");
		return(-1);
	}

	if((UCHARDATA(cca_image)[one_face]&typ_face)==0)
	{
		fprintf(stderr, "cca_get_simple_surface(): given pixel (%d) does not have given face (%d).\n", one_face, typ_face);
		return(-1);
	}

	data->face_type=typ_face;
	data->pixnumber=one_face;
	list_push_pointer(l, data);

	//Each time we add a face to surface, remove it from surface
	UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);

	//while the surface has faces to explore
	while(!list_isempty(l))
	{
		data=(face_desc*)list_pop_pointer(l);

		x=(data->pixnumber)%rs;
		y=((data->pixnumber)%ps)/rs;
		z=(data->pixnumber)/ps;

		face=data->face_type;
		px=data->pixnumber;

		//We don't need the data anymore
		free(data);

		//Look at the four edges of the face
		//The face has x edge ?
		switch(face)
		{
			case(CC_FXY):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FXY)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (1).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (2).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (3).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (4).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			case(CC_FXZ):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FXZ)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (2).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (5).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (6).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (7).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (8).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			case(CC_FYZ):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FYZ)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (3).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (9).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (10).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (11).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (12).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			default:
				fprintf(stderr, "cca_get_simple_surface(): Unrecognised face type.\n");
				return(-1);
		}
	}

	list_delete(l, FREE_DATA);

	return(0);
}


//All edges with cardinal sup or equal to 3 must have been removed from image
int32_t cca_get_simple_surface_old(struct xvimage *cca_image, complexe* result, uint32_t one_face, unsigned char typ_face)
{
	list *l, *container;
	face_desc *data;
	uint32_t rs, ps, x, y, z, px;
	unsigned char face;

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;

	l=list_newlist(MODE_FIFO, 1024);
	if(l==NULL)
	{
		fprintf(stderr, "cca_get_simple_surface(): Error, could not create new list.\n");
		return(-1);
	}

	data=(face_desc*)calloc(1, sizeof(face_desc));
	if(data==NULL)
	{
		fprintf(stderr, "cca_get_simple_surface(): data memory allocation error.\n");
		return(-1);
	}

	if((UCHARDATA(cca_image)[one_face]&typ_face)==0)
	{
		fprintf(stderr, "cca_get_simple_surface(): given pixel (%d) does not have given face (%d).\n", one_face, typ_face);
		return(-1);
	}

	data->face_type=typ_face;
	data->pixnumber=one_face;
	list_push_pointer(l, data);

	//Each time we add a face to surface, remove it from surface
	UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);

	//while the surface has faces to explore
	while(!list_isempty(l))
	{
		data=(face_desc*)list_pop_pointer(l);

		x=(data->pixnumber)%rs;
		y=((data->pixnumber)%ps)/rs;
		z=(data->pixnumber)/ps;

		face=data->face_type;
		px=data->pixnumber;

		//We don't need the data anymore
		free(data);

		//Look at the four edges of the face
		//The face has x edge ?
		switch(face)
		{
			case(CC_FXY):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FXY)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (1).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (2).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (3).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (4).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			case(CC_FXZ):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FXZ)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (2).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (5).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (6).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (7).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (8).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			case(CC_FYZ):
				//Add the face to the complexe structure
				if(complexe_add_element(result, px, CC_FYZ)<0)
				{
					fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (3).\n");
					return(-1);
				}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (9).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (10).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (11).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (12).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						//And add the face to the list
						list_push_pointer(l, data);
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			default:
				fprintf(stderr, "cca_get_simple_surface(): Unrecognised face type.\n");
				return(-1);
		}
	}

	list_delete(l, FREE_DATA);

	return(0);
}



int32_t cca_get_simple_local_surface(struct xvimage *cca_image, complexe* result, uint32_t px, unsigned char face)
{
	list *container;
	face_desc *data;
	uint32_t rs, ps, x, y, z;

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;

	x=px%rs;
	y=(px%ps)/rs;
	z=px/ps;

	UCHARDATA(cca_image)[px]=(UCHARDATA(cca_image)[px]|face)-face;


	//Look at the four edges of the face
	//The face has x edge ?
	switch(face)
	{
		case(CC_FXY):
				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (1).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (2).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (3).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (4).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

		case(CC_FXZ):
				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (5).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (6).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AX) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AX, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (7).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+1] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+1, x+1, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (8).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

		case(CC_FYZ):
				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (9).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px, x, y, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px, x, y, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (10).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+ps] & CC_AY) !=0)
					if(cca_cardinal_containers(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+ps, x, y, z+1, CC_AY, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (11).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				//If the edge of the face has only one face neighbour
				if( (UCHARDATA(cca_image)[px+rs] & CC_AZ) !=0)
					if(cca_cardinal_containers(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps) == 1)
					{
						//Find the neighbouring face
						container=cca_list_container(cca_image, px+rs, x, y+1, z, CC_AZ, rs, ps);
						if(container==NULL)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not build list container (12).\n");
							return(-1);
						}
						data=(face_desc*)list_pop_pointer(container);
						if(complexe_add_element(result, data->pixnumber, data->face_type)<0)
						{
							fprintf(stderr, "cca_get_simple_surface(): could not add element to complex structure (1).\n");
							return(-1);
						}
						UCHARDATA(cca_image)[data->pixnumber]=(UCHARDATA(cca_image)[data->pixnumber]|(data->face_type))-(data->face_type);
						list_delete(container, FREE_DATA);
					}

				break;

			default:
				fprintf(stderr, "cca_get_simple_surface(): Unrecognised face type.\n");
				return(-1);
	}

	return(0);
}


list* cca_simple_surface_segmentation_with_intersection_edges(struct xvimage* image, complexe* intersection_edges)
{
	complexe *temp;
	list *result;
	uint32_t i, rs, ps, N;


	result=list_newlist(MODE_FIFO, 1024);
	if(result==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new list.\n");
		return(NULL);
	}

	rs=rowsize(image);
	ps=rs*colsize(image);
	N=ps*depth(image);


	//***************************************************
	//Scan all vertex
	//**************************************************

	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (1).\n");
		return(NULL);
	}

	if(cca_scan_all_vertex_into_complex(image, temp)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while scanning vertex.\n");
		return(NULL);
	}

	list_push_pointer(result, temp);


	//*************************************************
	//Look for each surfaces... In order to do this, remove first all the surfaces intersections of the image
	//*************************************************

	//Remove the surfaces intersections
	for(i=0; i<intersection_edges->num_ax; i++)
	{
		cca_remove_complexface(image, CC_AX, intersection_edges->tab_ax[i], rs, ps);
	}
	for(i=0; i<intersection_edges->num_ay; i++)
	{
		cca_remove_complexface(image, CC_AY, intersection_edges->tab_ay[i], rs, ps);
	}
	for(i=0; i<intersection_edges->num_az; i++)
	{
		cca_remove_complexface(image, CC_AZ, intersection_edges->tab_az[i], rs, ps);
	}


	list_push_pointer(result, intersection_edges);


	//Then look for faces in the image
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_FXY) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (3).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXY)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (1).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(image)[i] & CC_FXZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (4).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (2).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(image)[i] & CC_FYZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (5).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FYZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (3).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}
	}


	//Regenerate image
	//Yes, we should do...

	return(result);
}


list* cca_simple_surface_segmentation_with_intersection_edges_and_overcollapse(struct xvimage* image, complexe* intersection_edges, struct xvimage* overcollapse)
{
	complexe *temp;
	list *result;
	uint32_t i, rs, ps, N, a, b, c, j;
	uint32_t num_surfaces, *backup_nums, modif;
	complexe **tab_complexe;
	cell *parcours;



	result=list_newlist(MODE_FIFO, 64);
	if(result==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new list.\n");
		return(NULL);
	}

	rs=rowsize(image);
	ps=rs*colsize(image);
	N=ps*depth(image);


	//***************************************************
	//Scan all vertex
	//**************************************************

	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (1).\n");
		return(NULL);
	}

	if(cca_scan_all_vertex_into_complex(image, temp)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while scanning vertex.\n");
		return(NULL);
	}

	list_push_pointer(result, temp);


	//*************************************************
	//Look for each surfaces... In order to do this, remove first all the surfaces intersections of the image
	//*************************************************

	//Remove the surfaces intersections
	for(i=0; i<intersection_edges->num_ax; i++)
	{
		UCHARDATA(image)[intersection_edges->tab_ax[i]]=(UCHARDATA(image)[intersection_edges->tab_ax[i]]|CC_AX)-CC_AX;
		UCHARDATA(overcollapse)[intersection_edges->tab_ax[i]]=(UCHARDATA(overcollapse)[intersection_edges->tab_ax[i]]|CC_AX)-CC_AX;
	}
	for(i=0; i<intersection_edges->num_ay; i++)
	{
		UCHARDATA(image)[intersection_edges->tab_ay[i]]=(UCHARDATA(image)[intersection_edges->tab_ay[i]]|CC_AY)-CC_AY;
		UCHARDATA(overcollapse)[intersection_edges->tab_ay[i]]=(UCHARDATA(overcollapse)[intersection_edges->tab_ay[i]]|CC_AY)-CC_AY;
	}
	for(i=0; i<intersection_edges->num_az; i++)
	{
		UCHARDATA(image)[intersection_edges->tab_az[i]]=(UCHARDATA(image)[intersection_edges->tab_az[i]]|CC_AZ)-CC_AZ;
		UCHARDATA(overcollapse)[intersection_edges->tab_az[i]]=(UCHARDATA(overcollapse)[intersection_edges->tab_az[i]]|CC_AZ)-CC_AZ;
	}


	list_push_pointer(result, intersection_edges);


	//Then look for faces in the image
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(overcollapse)[i] & CC_FXY) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (3).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(overcollapse, temp, i, CC_FXY)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (1).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(overcollapse)[i] & CC_FXZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (4).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(overcollapse, temp, i, CC_FXZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (2).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(overcollapse)[i] & CC_FYZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (5).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(overcollapse, temp, i, CC_FYZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (3).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}
	}



	num_surfaces=list_getsize(result)-2;
	tab_complexe=(complexe**)calloc(num_surfaces, sizeof(complexe*));
	backup_nums=(uint32_t*)calloc(3*num_surfaces, sizeof(uint32_t));


	parcours=list_gethead(result);

	for(i=0; i<num_surfaces+2; i++)
	{
		if(i>=2)
		{
			tab_complexe[i-2]=(complexe*)list_read_pointer(result, parcours);
		}
		parcours=list_gonext(parcours);
	}


	modif=1;

	while(modif==1)
	{
		modif=0;

		for(i=0; i<num_surfaces; i++)
		{
			a=tab_complexe[i]->num_fxy;
			b=tab_complexe[i]->num_fxz;
			c=tab_complexe[i]->num_fyz;

			for(j=backup_nums[3*i]; j<a; j++)
			{
				cca_get_simple_local_surface(image, tab_complexe[i], tab_complexe[i]->tab_fxy[j], CC_FXY);
			}

			for(j=backup_nums[3*i+1]; j<b; j++)
			{
				cca_get_simple_local_surface(image, tab_complexe[i], tab_complexe[i]->tab_fxz[j], CC_FXZ);
			}

			for(j=backup_nums[3*i+2]; j<c; j++)
			{
				cca_get_simple_local_surface(image, tab_complexe[i], tab_complexe[i]->tab_fyz[j], CC_FYZ);
			}

			if((a!=tab_complexe[i]->num_fxy) || (b!=tab_complexe[i]->num_fxz) || (c!=tab_complexe[i]->num_fyz))
			{
				modif=1;
			}

			backup_nums[3*i]=a;
			backup_nums[3*i+1]=b;
			backup_nums[3*i+2]=c;
		}
	}


	//Then look for faces in the image
	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_FXY) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (3).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXY)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (1).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(image)[i] & CC_FXZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (4).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FXZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (2).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}

		if( (UCHARDATA(image)[i] & CC_FYZ) != 0)
		{
			temp=complexe_new_complexe();
			if(temp==NULL)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (5).\n");
				return(NULL);
			}

			if(cca_get_simple_surface(image, temp, i, CC_FYZ)<0)
			{
				fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while computing simple surface (3).\n");
				return(NULL);
			}

			list_push_pointer(result, temp);
		}
	}


	//Regenerate image
	//Yes, we should do...

	return(result);
}


list* cca_decompose_into_connected_elements(struct xvimage* image)
{
	list *l;
	uint32_t rs, ps, N, i;
	unsigned char face;
	face_desc fd;
	complexe *r;

	l=list_newlist(MODE_LIFO, 64);
	if(l==NULL)
	{
		fprintf(stderr, "cca_decompose_into_connected_elements(): Could not create new list.\n");
		return(NULL);
	}

	rs=rowsize(image);
	ps=colsize(image)*rs;
	N=depth(image)*ps;
	for(i=0; i<N; i++)
	{
		for(face=CC_VOL; face>=CC_PT; face=face/2)
		{
			if( (UCHARDATA(image)[i] & face) !=0)
			{
				fd.pixnumber=i;
				fd.face_type=face;
				r=cca_geodilat_element_to_complex(&fd, image, rs, ps, N);
				if(r==NULL)
				{
					fprintf(stderr, "cca_decompose_into_connected_elements(): Error in subfunction.\n");
					return(NULL);
				}
				list_push_pointer(l, r);
			}
		}
	}

	return l;
}




/*struct xvimage* complexe_to_labelled_pgm(complexe *cpl, uint32_t label)
{
}*/


void complexe_geodilat_inside_cca(list *decomposition, struct xvimage *cca_image)
{
	complexe *cpl;
	face_desc f, *r;
	list *current_pixels;
	uint32_t i, j, tour, nb, num_complexe, rs, ps;
	list *complexe_and_pixels=list_newlist(MODE_FIFO, 1024);

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;

	num_complexe=list_getsize(decomposition);

	for(i=0; i<decomposition->size; i++)
	{
		cpl=(complexe*)list_pop_pointer(decomposition);

		list_push_pointer(complexe_and_pixels, cpl);
		list_push_pointer(decomposition, cpl);

		current_pixels=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 1024);
		for(j=0; j<cpl->num_pt_obj; j++) {f.pixnumber=cpl->tab_pt_obj[j]; f.face_type=CC_PT; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_ax; j++) {f.pixnumber=cpl->tab_ax[j]; f.face_type=CC_AX;if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_ay; j++) {f.pixnumber=cpl->tab_ay[j]; f.face_type=CC_AY; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_az; j++) {f.pixnumber=cpl->tab_az[j]; f.face_type=CC_AZ; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_fxy; j++) {f.pixnumber=cpl->tab_fxy[j]; f.face_type=CC_FXY; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_fxz; j++) {f.pixnumber=cpl->tab_fxz[j]; f.face_type=CC_FXZ; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_fyz; j++) {f.pixnumber=cpl->tab_fyz[j]; f.face_type=CC_FYZ; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}
		for(j=0; j<cpl->num_vol; j++) {f.pixnumber=cpl->tab_vol[j]; f.face_type=CC_VOL; if( (UCHARDATA(cca_image)[f.pixnumber] & f.face_type) !=0) list_push_pointer(current_pixels, &f);}

		complexe_remove_from_cca(cca_image, cpl);
		list_push_pointer(complexe_and_pixels, current_pixels);
	}

	tour=0;

	while(!list_isempty(complexe_and_pixels))
	{
		cpl=(complexe*)list_pop_pointer(complexe_and_pixels);
		current_pixels=(list*)list_pop_pointer(complexe_and_pixels);
		nb=list_getsize(current_pixels);
		if(nb!=0)
		{
			for(i=0; i<nb; i++)
			{
				r=(face_desc*)list_pop_pointer(current_pixels);
				if(tour>=num_complexe) complexe_add_element(cpl, r->pixnumber, r->face_type);
				cca_move_starfaces_to_list(cca_image, current_pixels, r, rs, ps);
				cca_move_subfaces_to_list(cca_image, current_pixels, r, rs, ps);
			}

			list_push_pointer(complexe_and_pixels, cpl);
			list_push_pointer(complexe_and_pixels, current_pixels);
		}
		else
		{
			list_delete(current_pixels, NO_FREE_DATA);
		}

		tour++;
	}

	list_delete(complexe_and_pixels, NO_FREE_DATA);

}


//Input image should be a complex (else, use at your own risk)
//Image will be changed by the function !
//Result will be list of complexes : list of elements PLUS intersection elements in first position
list *cca_decompose_skeleton_into_elements(struct xvimage* image)
{
	list *result;
	complexe *intersection;
	unsigned char face;
	uint32_t i, N, rs, cs, d, ps, border_code, j, k, pix;

	//First, find the intersection elements and add them to a complex structure
	cca_keep_only_cells(image);
	intersection=complexe_new_complexe();
	if(intersection == NULL)
	{
		fprintf(stderr, "cca_decompose_skeleton_into_elements(): Error when creating new complex.\n");
		return(NULL);
	}

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;
	d=depth(image);
	N=ps*d;

	pix=0;
	for(k=0; k<d; k++)
	for(j=0; j<cs; j++)
	for(i=0; i<rs; i++)
	{
		if(	UCHARDATA(image)[pix] != 0 ||
			(i>0 && UCHARDATA(image)[pix-1] != 0) ||
			(j>0 && UCHARDATA(image)[pix-rs] != 0) ||
			(k>0 && UCHARDATA(image)[pix-ps] != 0) ||
			(i>0 && j>0 && UCHARDATA(image)[pix-1-rs] != 0) ||
			(i>0 && k>0 && UCHARDATA(image)[pix-1-ps] != 0) ||
			(k>0 && j>0 && UCHARDATA(image)[pix-ps-rs] != 0) ||
			(k>0 && j>0 && i>0 && UCHARDATA(image)[pix-1-ps-rs] != 0) )
			{
				for(face=CC_VOL/2; face>0; face=face/2)
				{
					border_code=cca_is_face_onborder_v2(face, pix, rs, cs, d);
					if(cca_face_is_an_intersection_face(image, face, pix, rs, ps, border_code))
						complexe_add_element(intersection, pix, face);
				}
			}
		pix++;
	}


	//Get back the normal image, and remove the intersection elements from it
	cca_makecomplex(image);
	for(i=0; i<intersection->num_pt_obj; i++) UCHARDATA(image)[intersection->tab_pt_obj[i]] = UCHARDATA(image)[intersection->tab_pt_obj[i]] - CC_PT;
	for(i=0; i<intersection->num_ax; i++) UCHARDATA(image)[intersection->tab_ax[i]] = UCHARDATA(image)[intersection->tab_ax[i]] - CC_AX;
	for(i=0; i<intersection->num_ay; i++) UCHARDATA(image)[intersection->tab_ay[i]] = UCHARDATA(image)[intersection->tab_ay[i]] - CC_AY;
	for(i=0; i<intersection->num_az; i++) UCHARDATA(image)[intersection->tab_az[i]] = UCHARDATA(image)[intersection->tab_az[i]] - CC_AZ;
	for(i=0; i<intersection->num_fxy; i++) UCHARDATA(image)[intersection->tab_fxy[i]] = UCHARDATA(image)[intersection->tab_fxy[i]] - CC_FXY;
	for(i=0; i<intersection->num_fxz; i++) UCHARDATA(image)[intersection->tab_fxz[i]] = UCHARDATA(image)[intersection->tab_fxz[i]] - CC_FXZ;
	for(i=0; i<intersection->num_fyz; i++) UCHARDATA(image)[intersection->tab_fyz[i]] = UCHARDATA(image)[intersection->tab_fyz[i]] - CC_FYZ;


	//Now, perform decomposition
	result=cca_decompose_into_connected_elements(image);
	if(result==NULL)
	{
		fprintf(stderr, "cca_decompose_skeleton_into_elements(): Error during skeleton decomposition.\n");
		complexe_free_complexe(intersection);
		return(NULL);
	}

	//Finally, add the intersection to the head of the list
	list_push_pointer(result, intersection);

	return(result);
}




list* cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image(struct xvimage* image, complexe* intersection_edges, struct xvimage *keep_reject_image, unsigned char strategy)
{
	list *filt_surf;
	complexe *cpl;
	uint32_t i, keep;

	list *result=cca_simple_surface_segmentation_with_intersection_edges(image, intersection_edges);
	if(result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation_with_intersection_edges() failed.\n");
		return(NULL);
	}

	filt_surf=list_newlist(MODE_FIFO, 64);
	if(filt_surf==NULL)
	{
		fprintf(stderr, "Error: New list creation failed.\n");
		while(! list_isempty(result))
		{
			cpl=(complexe*)list_pop_pointer(result);
			complexe_free_complexe(cpl);
		}
		list_delete(result, NO_FREE_DATA);
		return(NULL);
	}


	//There is at least two complexes in this list : the list of vertices and the list of intersect edges
	for(i=0; i<2 && !list_isempty(result); i++)
	{
		cpl=(complexe*)list_pop_pointer(result);
		list_push_pointer(filt_surf, cpl);
	}

	//Scan each surface and check if it should be kept or not
	while(! list_isempty(result))
	{
		cpl=(complexe*)list_pop_pointer(result);

		keep=1;
		if(keep_reject_image!=NULL)
		{
			switch(strategy)
			{
				case(KEEP):
					if(complexe_intersects_image(cpl, keep_reject_image))
					{
						keep=1;
						//Remove from complex all faces which do not belong do keep_image
						for(i=0; i<cpl->num_fxy; i++)
						{

							if( (UCHARDATA(keep_reject_image)[cpl->tab_fxy[i]] & CC_FXY) ==0)
							{
								cpl->tab_fxy[i] = cpl->tab_fxy[cpl->num_fxy -1];
								cpl->num_fxy = cpl->num_fxy -1;
								i--;
							}
						}

						for(i=0; i<cpl->num_fxz; i++)
						{
							if( (UCHARDATA(keep_reject_image)[cpl->tab_fxz[i]] & CC_FXZ) ==0)
							{
								cpl->tab_fxz[i] = cpl->tab_fxz[cpl->num_fxz -1];
								cpl->num_fxz = cpl->num_fxz -1;
								i--;
							}
						}

						for(i=0; i<cpl->num_fyz; i++)
						{
							if( (UCHARDATA(keep_reject_image)[cpl->tab_fyz[i]] & CC_FYZ) ==0)
							{
								cpl->tab_fyz[i] = cpl->tab_fyz[cpl->num_fyz -1];
								cpl->num_fyz = cpl->num_fyz -1;
								i--;
							}
						}
					}

					else
						keep=0;
					break;

				case(REJECT):
					if(complexe_intersects_image(cpl, keep_reject_image))
						keep=0;
					else
						keep=1;
					break;
			}
		}

		if(keep==1)
			list_push_pointer(filt_surf, cpl);
		else
			complexe_free_complexe(cpl);
	}

	list_delete(result, NO_FREE_DATA);

	return(filt_surf);
}



list* cca_simple_surface_segmentation(struct xvimage* image)
{
	complexe *temp;



	//**************************************************************
	//Now, we create a new object with all surfaces intersections
	//**************************************************************
	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation(): could not create new complexe.\n");
		return(NULL);
	}

	if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 3)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation(): error while scanning for surfaces intersections (2).\n");
		return(NULL);
	}

	if(cca_get_all_edges_with_given_cardinalcontainer(image, temp, 4)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation(): error while scanning for surfaces intersections (3).\n");
		return(NULL);
	}

	return(cca_simple_surface_segmentation_with_intersection_edges(image, temp));

}


list* cca_simple_curve_decomposition(struct xvimage* edges, complexe *non_regular_vertex)
{
	uint32_t i, rs, ps, N;
	list *result, *cont;
	complexe *temp;
	face_desc d, *h;
	unsigned char f;


	result=list_newlist(MODE_FIFO, 128);
	if(result==NULL)
	{
		fprintf(stderr, "cca_simple_curve_decomposition(): could not create new list.\n");
		return(NULL);
	}


	rs=rowsize(edges);
	ps=rs*colsize(edges);
	N=ps*depth(edges);


	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (1).\n");
		return(NULL);
	}

	if(cca_scan_all_vertex_into_complex(edges, temp)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while scanning vertex.\n");
		return(NULL);
	}

	list_push_pointer(result, temp);


	//Remove the curves intersections
	for(i=0; i<non_regular_vertex->num_pt_obj; i++)
	{
		UCHARDATA(edges)[non_regular_vertex->tab_pt_obj[i]]&=(255-CC_PT);
	}


	//Go, look for each edge and build the curve it belongs to
	for(i=0; i<non_regular_vertex->num_pt_obj; i++)
	{
		cont=cca_list_starface_dimension_direct_over(non_regular_vertex->tab_pt_obj[i], CC_PT, rs, ps/rs, N/ps);
		if(cont==NULL)
		{
			fprintf(stderr, "cca_simple_curve_decomposition(): error in subfunction.\n");
			return(NULL);
		}

		while(!list_isempty(cont))
		{
			h=(face_desc*)list_pop_pointer(cont);

			if( (UCHARDATA(edges)[h->pixnumber] & h->face_type) !=0)
			{
				temp=cca_geodilat_simplecurve_to_complex(h, edges, rs, ps, N);
				if(temp==NULL)
				{
					fprintf(stderr, "cca_simple_curve_decomposition(): error while calling subfunction.\n");
					return(NULL);
				}
				//complexe_remove_faces(temp, CC_PT);
				list_push_pointer(result, temp);
			}
		}

		list_delete(cont, NO_FREE_DATA);
	}

	//Remaining edges would be cycles
	for(i=0; i<N; i++)
	{
		if(UCHARDATA(edges)[i]!=0)
		{
			for(f=CC_AZ; f<=CC_AX; f=f*2)
			{
				if( (UCHARDATA(edges)[i] & f) !=0)
				{
					UCHARDATA(edges)[i]&=(255-CC_PT); //Remove one point in order to get a nice cycle in the enumeration
					d.face_type=f;
					d.pixnumber=i;
					temp=cca_geodilat_simplecurve_to_complex(&d, edges, rs, ps, N);
					if(temp==NULL)
					{
						fprintf(stderr, "cca_simple_curve_decomposition(): error while calling subfunction.\n");
						return(NULL);
					}
					//complexe_remove_faces(temp, CC_PT);
					list_push_pointer(result, temp);
				}
			}
		}
	}

	return(result);
}



list* cca_simple_curve_decomposition_non_optimized(struct xvimage* edges, complexe *non_regular_vertex)
{
	uint32_t i, rs, ps, N;
	list *result;
	complexe *temp;
	face_desc d;
	unsigned char f;


	result=list_newlist(MODE_FIFO, 128);
	if(result==NULL)
	{
		fprintf(stderr, "cca_simple_curve_decomposition(): could not create new list.\n");
		return(NULL);
	}


	rs=rowsize(edges);
	ps=rs*colsize(edges);
	N=ps*depth(edges);


	temp=complexe_new_complexe();
	if(temp==NULL)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): could not create new complexe (1).\n");
		return(NULL);
	}

	if(cca_scan_all_vertex_into_complex(edges, temp)<0)
	{
		fprintf(stderr, "cca_simple_surface_segmentation_with_intersection_edges(): error while scanning vertex.\n");
		return(NULL);
	}

	list_push_pointer(result, temp);


	//Remove the curves intersections
	for(i=0; i<non_regular_vertex->num_pt_obj; i++)
	{
		UCHARDATA(edges)[non_regular_vertex->tab_pt_obj[i]]&=(255-CC_PT);
	}

	for(i=0; i<N; i++)
	{
		if(UCHARDATA(edges)[i]!=0)
		{
			for(f=CC_AZ; f<=CC_AX; f=f*2)
			{
				if( (UCHARDATA(edges)[i] & f) !=0)
				{
					d.face_type=f;
					d.pixnumber=i;
					temp=cca_geodilat_element_to_complex(&d, edges, rs, ps, N);
					if(temp==NULL)
					{
						fprintf(stderr, "cca_simple_curve_decomposition(): error while calling subfunction.\n");
						return(NULL);
					}
					//complexe_remove_faces(temp, CC_PT);
					list_push_pointer(result, temp);
				}
			}
		}
	}

	return(result);
}



