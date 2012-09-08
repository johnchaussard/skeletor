//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>
#include <mcimage.h>

//Librairie perso
#include <libcomplex.h>



int32_t complexe_add_element_internal(uint32_t data, uint32_t *num_elem, uint32_t **tab_elem)
{
	if ((*num_elem)%BASE_ALLOC==0)
	{
		(*tab_elem)=(uint32_t *)realloc(*tab_elem, ((*num_elem)+BASE_ALLOC)*sizeof(uint32_t));
		if(*tab_elem == NULL)
		{
			fprintf(stderr, "complexe_add_element(): Memory allocation error.\n");
			return(-1);
		}
	}

	(*tab_elem)[*num_elem]=data;
	*num_elem=*num_elem +1;

	return(0);
}


int32_t complexe_add_array_element_internal(uint32_t *data, uint32_t num_data, uint32_t *num_elem, uint32_t **tab_elem)
{
	uint32_t max_elem, new_max_elem;

	max_elem=(*num_elem)/BASE_ALLOC;
	if((*num_elem)%BASE_ALLOC) max_elem++;
	max_elem*=BASE_ALLOC;

	new_max_elem=(*num_elem + num_data)/BASE_ALLOC;
	if((*num_elem + num_data)%BASE_ALLOC) new_max_elem++;
	new_max_elem*=BASE_ALLOC;

	if( (new_max_elem - max_elem)>=BASE_ALLOC)
	{
		(*tab_elem)=(uint32_t *)realloc(*tab_elem, new_max_elem*sizeof(uint32_t));
		if(*tab_elem == NULL)
		{
			fprintf(stderr, "complexe_add_array_element(): Memory allocation error.\n");
			return(-1);
		}
	}

	memcpy(&(*tab_elem)[*num_elem], data, num_data*sizeof(uint32_t));
	*num_elem=*num_elem +num_data;

	return(0);
}


uint32_t complexe_get_numvertex_of_element(complexe *cpl, uint32_t num_element, unsigned char typ_face)
{
	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			fprintf(stdout, "complexe_get_numvertex_of_element(): This function is for GENERAL_COMPLEX.\n");
			return(1);
			break;

		case(GENERAL_COMPLEXE):
			switch(typ_face)
			{
				case(GC_PT):
					if(num_element < cpl->num_pt_obj)
						return (1);
					else
						return (0);
					break;

				case(GC_EDGE):
					if(num_element >= cpl->num_edges)
						return (0);
					else if(num_element>0)
						return (cpl->tab_edges[num_element] - cpl->tab_edges[num_element-1]);
					else
						return (cpl->tab_edges[num_element]);
					break;

				case(GC_FACE):
					if(num_element >= cpl->num_faces)
						return (0);
					else if(num_element>0)
						return (cpl->tab_faces[num_element] - cpl->tab_faces[num_element-1]);
					else
						return (cpl->tab_faces[num_element]);
					break;

				default:
					fprintf(stderr, "complexe_get_numvertex_of_element(): Unknown face type.\n");
					return(0);
					break;
			}

		default:
			fprintf(stderr, "complexe_get_numvertex_of_element(): Unknown complex type.\n");
			return(0);
			break;
	}

	return(0);
}


//No possible error code given back
uint32_t complexe_get_vertex_of_element(complexe *cpl, uint32_t num_element, uint32_t num_vertex, unsigned char typ_face)
{
	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			fprintf(stdout, "complexe_get_vertex_of_element(): This function is for GENERAL_COMPLEX.\n");
			return(1);
			break;

		case(GENERAL_COMPLEXE):
			switch(typ_face)
			{
				case(GC_EDGE):
					return (cpl->tab_edges_pts[complexe_get_vertex_position_in_element(cpl, num_element, num_vertex, typ_face)]);
					break;

				case(GC_FACE):
					return (cpl->tab_faces_pts[complexe_get_vertex_position_in_element(cpl, num_element, num_vertex, typ_face)]);
					break;

				default:
					fprintf(stderr, "complexe_get_vertex_of_element(): Unknown face type.\n");
					return(0);
					break;
			}

		default:
			fprintf(stderr, "complexe_get_vertex_of_element(): Unknown complex type.\n");
			return(0);
			break;
	}

	return(0);
}



//No possible error code given back
uint32_t complexe_get_vertex_position_in_element(complexe *cpl, uint32_t num_element, uint32_t num_vertex, unsigned char typ_face)
{
	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			fprintf(stdout, "complexe_get_vertex_position_in_element(): This function is for GENERAL_COMPLEX.\n");
			return(1);
			break;

		case(GENERAL_COMPLEXE):
			switch(typ_face)
			{
				case(GC_EDGE):
					if(num_element >= cpl->num_edges)
						return (0);
					else if(num_element>0)
					{
						assert( num_vertex + cpl->tab_edges[num_element-1] < cpl->tab_edges[num_element]);
						return (num_vertex + cpl->tab_edges[num_element-1]);
					}
					else
					{
						assert( num_vertex < cpl->tab_edges[num_element]);
						return (num_vertex);
					}
					break;

				case(GC_FACE):
					if(num_element >= cpl->num_faces)
						return (0);
					else if(num_element>0)
					{
						assert( num_vertex + cpl->tab_faces[num_element-1] < cpl->tab_faces[num_element]);
						return (num_vertex + cpl->tab_faces[num_element-1]);
					}
					else
					{
						assert( num_vertex < cpl->tab_faces[num_element]);
						return (num_vertex);
					}
					break;

				default:
					fprintf(stderr, "complexe_get_vertex_position_in_element(): Unknown face type.\n");
					return(0);
					break;
			}

		default:
			fprintf(stderr, "complexe_get_vertex_position_in_element(): Unknown complex type.\n");
			return(0);
			break;
	}

	return(0);
}




int32_t complexe_close_element(complexe *cpl, unsigned char typ_element)
{
	if(cpl->TYP_COMPLEXE != GENERAL_COMPLEXE)
	{
		fprintf(stdout, "complexe_close_element(): Warning, function is only for GENERAL_COMPLEX.\n");
		return 0;
	}

	switch(typ_element)
	{
		case(GC_PT):
			break;

		case(GC_EDGE):
			return(complexe_add_element_internal(cpl->num_edges_pts, &(cpl->num_edges), &(cpl->tab_edges)));
			break;

		case(GC_FACE):
			return(complexe_add_element_internal(cpl->num_faces_pts, &(cpl->num_faces), &(cpl->tab_faces)));
			break;

		default:
			fprintf(stderr, "complexe_close_element(): error, Unknown element.\n");
			return(-1);
			break;
	}

	return(0);
}


int32_t complexe_add_element(complexe *cpl, uint32_t data, unsigned char typ_face)
{
	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			switch(typ_face)
			{
				case(CC_PT):
					return(complexe_add_element_internal(data, &(cpl->num_pt_obj), &(cpl->tab_pt_obj)));
					break;

				case(CC_AX):
					return(complexe_add_element_internal(data, &(cpl->num_ax), &(cpl->tab_ax)));
					break;

				case(CC_AY):
					return(complexe_add_element_internal(data, &(cpl->num_ay), &(cpl->tab_ay)));
					break;

				case(CC_AZ):
					return(complexe_add_element_internal(data, &(cpl->num_az), &(cpl->tab_az)));
					break;

				case(CC_FXY):
					return(complexe_add_element_internal(data, &(cpl->num_fxy), &(cpl->tab_fxy)));
					break;

				case(CC_FXZ):
					return(complexe_add_element_internal(data, &(cpl->num_fxz), &(cpl->tab_fxz)));
					break;

				case(CC_FYZ):
					return(complexe_add_element_internal(data, &(cpl->num_fyz), &(cpl->tab_fyz)));
					break;

				case(CC_VOL):
					return(complexe_add_element_internal(data, &(cpl->num_vol), &(cpl->tab_vol)));
					break;

				default:
					fprintf(stderr, "complexe_add_element(): Unknown face type given.\n");
					return(-1);
			}
			break;

		case(GENERAL_COMPLEXE):
			switch(typ_face)
			{
				case(GC_PT):
					return(complexe_add_element_internal(data, &(cpl->num_pt_obj), &(cpl->tab_pt_obj)));
					break;

				case(GC_EDGE):
					return(complexe_add_element_internal(data, &(cpl->num_edges_pts), &(cpl->tab_edges_pts)));
					break;

				case(GC_FACE):
					return(complexe_add_element_internal(data, &(cpl->num_faces_pts), &(cpl->tab_faces_pts)));
					break;

				default:
					fprintf(stderr, "complexe_add_element(): Unknown face type given.\n");
					return(-1);
			}

		default:
			fprintf(stderr, "complexe_add_element(): Unknown complexe type given.\n");
			return(-1);

	}
	return(0);
}


int32_t complexe_add_array_element(complexe *cpl, uint32_t* data, uint32_t num_element_to_add, unsigned char typ_face)
{
	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			switch(typ_face)
			{
				case(CC_PT):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_pt_obj), &(cpl->tab_pt_obj)));
					break;

				case(CC_AX):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_ax), &(cpl->tab_ax)));
					break;

				case(CC_AY):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_ay), &(cpl->tab_ay)));
					break;

				case(CC_AZ):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_az), &(cpl->tab_az)));
					break;

				case(CC_FXY):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_fxy), &(cpl->tab_fxy)));
					break;

				case(CC_FXZ):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_fxz), &(cpl->tab_fxz)));
					break;

				case(CC_FYZ):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_fyz), &(cpl->tab_fyz)));
					break;

				case(CC_VOL):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_vol), &(cpl->tab_vol)));
					break;

				default:
					fprintf(stderr, "complexe_add_array_element(): Unknown face type given.\n");
					return(-1);
			}
			break;

		case(GENERAL_COMPLEXE):
			switch(typ_face)
			{
				case(GC_PT):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_pt_obj), &(cpl->tab_pt_obj)));
					break;

				case(GC_EDGE):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_edges_pts), &(cpl->tab_edges_pts)));
					break;

				case(GC_FACE):
					return(complexe_add_array_element_internal(data, num_element_to_add, &(cpl->num_faces_pts), &(cpl->tab_faces_pts)));
					break;

				default:
					fprintf(stderr, "complexe_add_array_element(): Unknown face type given.\n");
					return(-1);
			}

		default:
			fprintf(stderr, "complexe_add_array_element(): Unknown complexe type given.\n");
			return(-1);

	}
	return(0);
}


complexe *complexe_new_complexe()
{
	complexe *result;

	result=(complexe*)calloc(1, sizeof(complexe));
	if(result==NULL)
	{
		fprintf(stderr, "complexe_new_complexe(): Memory allocation error.\n");
		return(NULL);
	}
	return(result);
}


void complexe_free_complexe(complexe* data)
{
	complexe_remove_faces(data, CC_PT);
	complexe_remove_faces(data, CC_AX);
	complexe_remove_faces(data, CC_AY);
	complexe_remove_faces(data, CC_AZ);
	complexe_remove_faces(data, CC_FXY);
	complexe_remove_faces(data, CC_FYZ);
	complexe_remove_faces(data, CC_FXZ);
	complexe_remove_faces(data, CC_VOL);
	complexe_remove_faces(data, GC_FACE);
	complexe_remove_faces(data, GC_EDGE);
	free(data);
}


//Function used in qsort for having a sorted array
int32_t qsort_function_on_uint32(const void* inta, const void* intb)
{
	if((* (uint32_t*)(inta)) < (* (uint32_t*)(intb)))
		return (-1);
	else if((* (uint32_t*)(inta)) > (* (uint32_t*)(intb)))
		return(1);
	else
		return(0);
}


int32_t qsort_function_on_float(const void* inta, const void* intb)
{
	if((* (float*)(inta)) < (* (float*)(intb)))
		return (-1);
	else if((* (float*)(inta)) > (* (float*)(intb)))
		return(1);
	else
		return(0);
}





void complexe_compute_vertex_array(complexe *data, uint32_t rs, uint32_t ps)
{
	uint32_t i, *rd, *wr, n;

	switch(data->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			//AX edges
			for(i=0; i<data->num_ax; i++)
			{
				complexe_add_element(data, data->tab_ax[i], CC_PT);
				complexe_add_element(data, data->tab_ax[i]+1, CC_PT);
			}

			//AY edges
			for(i=0; i<data->num_ay; i++)
			{
				complexe_add_element(data, data->tab_ay[i], CC_PT);
				complexe_add_element(data, data->tab_ay[i]+rs, CC_PT);
			}

			//AZ edges
			for(i=0; i<data->num_az; i++)
			{
				complexe_add_element(data, data->tab_az[i], CC_PT);
				complexe_add_element(data, data->tab_az[i]+ps, CC_PT);
			}

			//XY faces
			for(i=0; i<data->num_fxy; i++)
			{
				complexe_add_element(data, data->tab_fxy[i], CC_PT);
				complexe_add_element(data, data->tab_fxy[i]+1, CC_PT);
				complexe_add_element(data, data->tab_fxy[i]+rs, CC_PT);
				complexe_add_element(data, data->tab_fxy[i]+1+rs, CC_PT);
			}

			//XZ faces
			for(i=0; i<data->num_fxz; i++)
			{
				complexe_add_element(data, data->tab_fxz[i], CC_PT);
				complexe_add_element(data, data->tab_fxz[i]+1, CC_PT);
				complexe_add_element(data, data->tab_fxz[i]+ps, CC_PT);
				complexe_add_element(data, data->tab_fxz[i]+1+ps, CC_PT);
			}

			//YZ faces
			for(i=0; i<data->num_fyz; i++)
			{
				complexe_add_element(data, data->tab_fyz[i], CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+rs, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+ps, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+rs+ps, CC_PT);
			}

			//Volumes
			for(i=0; i<data->num_vol; i++)
			{
				complexe_add_element(data, data->tab_fyz[i], CC_PT);
				complexe_add_element(data, data->tab_fxz[i]+1, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+rs, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+ps, CC_PT);
				complexe_add_element(data, data->tab_fxy[i]+1+rs, CC_PT);
				complexe_add_element(data, data->tab_fxz[i]+1+ps, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+rs+ps, CC_PT);
				complexe_add_element(data, data->tab_fyz[i]+1+rs+ps, CC_PT);
			}
			break;

		case(GENERAL_COMPLEXE):
			for(i=0; i<data->num_edges_pts; i++)
			{
				complexe_add_element(data, data->tab_edges_pts[i], GC_PT);
			}
			for(i=0; i<data->num_faces_pts; i++)
			{
				complexe_add_element(data, data->tab_faces_pts[i], GC_PT);
			}
			break;

		default:
			break;
	}


	if(data->num_pt_obj>0)
	{
		qsort(&(data->tab_pt_obj[0]), data->num_pt_obj, sizeof(uint32_t), qsort_function_on_uint32);

		//Remove doubles
		wr=&(data->tab_pt_obj[0]);
		rd=&(data->tab_pt_obj[1]);
		n=1;
		for(i=1; i<data->num_pt_obj; i++)
		{
			if(*wr != *rd)
			{
				wr++;
				*wr=*rd;
				n++;
			}
			rd++;
		}

		//Resize array
		(data->tab_pt_obj)=(uint32_t*)realloc(&(data->tab_pt_obj[0]), ((n/BASE_ALLOC)+1)*BASE_ALLOC*sizeof(uint32_t));
		data->num_pt_obj=n;

	}
}


void complexe_remove_faces(complexe *data, unsigned char typ_face)
{
	switch(typ_face)
	{
		case(CC_PT):
			if(data->num_pt_obj >0)
			{
				data->num_pt_obj=0;
				free(data->tab_pt_obj);
				data->tab_pt_obj=NULL;
			}
			break;

		case(CC_AX):
			if(data->num_ax >0)
			{
				data->num_ax=0;
				free(data->tab_ax);
				data->tab_ax=NULL;
			}
			break;

		case(CC_AY):
			if(data->num_ay >0)
			{
				data->num_ay=0;
				free(data->tab_ay);
				data->tab_ay=NULL;
			}
			break;

		case(CC_AZ):
			if(data->num_az >0)
			{
				data->num_az=0;
				free(data->tab_az);
				data->tab_az=NULL;
			}
			break;

		case(CC_FXY):
			if(data->num_fxy >0)
			{
				data->num_fxy=0;
				free(data->tab_fxy);
				data->tab_fxy=NULL;
			}
			break;

		case(CC_FXZ):
			if(data->num_fxz >0)
			{
				data->num_fxz=0;
				free(data->tab_fxz);
				data->tab_fxz=NULL;
			}
			break;

		case(CC_FYZ):
			if(data->num_fyz >0)
			{
				data->num_fyz=0;
				free(data->tab_fyz);
				data->tab_fyz=NULL;
			}
			break;

		case(CC_VOL):
			if(data->num_vol >0)
			{
				data->num_vol=0;
				free(data->tab_vol);
				data->tab_vol=NULL;
			}
			break;

		case(GC_EDGE):
			if(data->num_edges_pts>0)
			{
				data->num_edges_pts=0;
				data->num_edges=0;
				free(data->tab_edges);
				free(data->tab_edges_pts);
				data->tab_edges=NULL;
				data->tab_edges_pts=NULL;
			}
			break;

		case(GC_FACE):
			if(data->num_faces_pts>0)
			{
				data->num_faces_pts=0;
				data->num_faces=0;
				free(data->tab_faces);
				free(data->tab_faces_pts);
				data->tab_faces=NULL;
				data->tab_faces_pts=NULL;
			}
			break;

		default:
			break;
	}
}
