//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>
#include <mcimage.h>
#include <ltangents.h>

//Librairie perso
#include <libcomplex.h>


#define EPSILON 0.000001


//Input complex must have vertex array
complexe* complexe_downsample_point_resolution(complexe *c, uint32_t rs, uint32_t ps, double ratio)
{
	uint32_t i, new_ps, new_rs;
	uint32_t *new_coord;
	complexe *result;

	if(ratio<=1.0)
	{
		fprintf(stdout, "complexe_downsample_point_resolution(): Warning, the ratio should be greater than 1 as this function downsamples resolution.\n");
		return c;
	}

	new_coord=(uint32_t*)calloc(c->num_pt_obj, 2*sizeof(uint32_t));
	if(new_coord==NULL)
	{
		fprintf(stderr, "complexe_downsample_point_resolution(): Memory allocation error.\n");
		return NULL;
	}

	new_rs=(uint32_t)(rs/ratio);
	new_ps=(uint32_t)(new_rs * ((ps/rs)/ratio));

	for(i=0; i<c->num_pt_obj; i++)
	{
		new_coord[2*i+1]=c->tab_pt_obj[i];
		new_coord[2*i]=	(uint32_t)((double)(getxfrompixnum(new_coord[2*i+1], rs, ps)) /ratio) +
							(uint32_t)((double)(getyfrompixnum(new_coord[2*i+1], rs, ps)) /ratio)*new_rs +
							(uint32_t)((double)(getzfrompixnum(new_coord[2*i+1], rs, ps)) /ratio)*new_ps;
	}

	qsort(new_coord, c->num_pt_obj, 2*sizeof(uint32_t), qsort_function_on_uint32);

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "complexe_downsample_point_resolution(): Memory allocation error (2).\n");
		free(new_coord);
		return(NULL);
	}

	if(c->num_pt_obj >0)
		complexe_add_element(result, new_coord[1], CC_PT);

	for(i=1; i<c->num_pt_obj; i++)
	{
		if(new_coord[2*i] != new_coord[2*i-2])
			complexe_add_element(result, new_coord[2*i+1], CC_PT);
	}

	free(new_coord);

	return(result);
}




uint32_t complexe_intersects_image(complexe *c, struct xvimage *i)
{
	uint32_t j;

	for(j=0; j<c->num_pt_obj; j++)
		if( (UCHARDATA(i)[c->tab_pt_obj[j]] & CC_PT) != 0)
			return 1;

	for(j=0; j<c->num_ax; j++)
		if( (UCHARDATA(i)[c->tab_ax[j]] & CC_AX) != 0)
			return 1;

	for(j=0; j<c->num_ay; j++)
		if( (UCHARDATA(i)[c->tab_ay[j]] & CC_AY) != 0)
			return 1;

	for(j=0; j<c->num_az; j++)
		if( (UCHARDATA(i)[c->tab_az[j]] & CC_AZ) != 0)
			return 1;

	for(j=0; j<c->num_fxy; j++)
		if( (UCHARDATA(i)[c->tab_fxy[j]] & CC_FXY) != 0)
			return 1;

	for(j=0; j<c->num_fxz; j++)
		if( (UCHARDATA(i)[c->tab_fxz[j]] & CC_FXZ) != 0)
			return 1;

	for(j=0; j<c->num_fyz; j++)
		if( (UCHARDATA(i)[c->tab_fyz[j]] & CC_FYZ) != 0)
			return 1;

	for(j=0; j<c->num_vol; j++)
		if( (UCHARDATA(i)[c->tab_vol[j]] & CC_VOL) != 0)
			return 1;

	return 0;
}

//Input : a face g belonging to a free pair (f,g)
//Output : f (or EMPTY if g is not free)
face_desc cca_get_container_of_free_face(unsigned char facetype, uint32_t pixnumber, uint32_t direction, uint32_t rs, uint32_t ps)
{
	face_desc r;

	if(!ISFREE(facetype, direction))
	{
		r.face_type=EMPTY;
		r.pixnumber=0;
		return r;
	}

	switch(facetype)
	{
		case(CC_PT):
			if(direction==CC_VERT_COLL_XUP)
			{
				r.face_type=CC_AX;
				r.pixnumber=pixnumber;
			}
			else if(direction==CC_VERT_COLL_XDOWN)
			{
				r.face_type=CC_AX;
				r.pixnumber=pixnumber-1;
			}
			else if(direction==CC_VERT_COLL_YUP)
			{
				r.face_type=CC_AY;
				r.pixnumber=pixnumber;
			}
			else if(direction==CC_VERT_COLL_YDOWN)
			{
				r.face_type=CC_AY;
				r.pixnumber=pixnumber-rs;
			}
			else if(direction==CC_VERT_COLL_ZUP)
			{
				r.face_type=CC_AZ;
				r.pixnumber=pixnumber;
			}
			else
			{
				r.face_type=CC_AZ;
				r.pixnumber=pixnumber-ps;
			}
		break;

		case(CC_AX):
			if(direction==CC_EDGE_COLL_DIR1)
			{
				r.face_type=CC_FXY;
				r.pixnumber=pixnumber;
			}
			else if(direction==CC_EDGE_COLL_DIR2)
			{
				r.face_type=CC_FXY;
				r.pixnumber=pixnumber-rs;
			}
			else if(direction==CC_EDGE_COLL_DIR3)
			{
				r.face_type=CC_FXZ;
				r.pixnumber=pixnumber;
			}
			else
			{
				r.face_type=CC_FXZ;
				r.pixnumber=pixnumber-ps;
			}
		break;

		case(CC_AY):
			if(direction==CC_EDGE_COLL_DIR1)
			{
				r.face_type=CC_FXY;
				r.pixnumber=pixnumber;
			}
			else if(direction==CC_EDGE_COLL_DIR2)
			{
				r.face_type=CC_FXY;
				r.pixnumber=pixnumber-1;
			}
			else if(direction==CC_EDGE_COLL_DIR3)
			{
				r.face_type=CC_FYZ;
				r.pixnumber=pixnumber;
			}
			else
			{
				r.face_type=CC_FYZ;
				r.pixnumber=pixnumber-ps;
			}
		break;

		case(CC_AZ):
			if(direction==CC_EDGE_COLL_DIR1)
			{
				r.face_type=CC_FXZ;
				r.pixnumber=pixnumber;
			}
			else if(direction==CC_EDGE_COLL_DIR2)
			{
				r.face_type=CC_FXZ;
				r.pixnumber=pixnumber-1;
			}
			else if(direction==CC_EDGE_COLL_DIR3)
			{
				r.face_type=CC_FYZ;
				r.pixnumber=pixnumber;
			}
			else
			{
				r.face_type=CC_FYZ;
				r.pixnumber=pixnumber-rs;
			}
		break;

		case(CC_FXY):
			r.face_type=CC_VOL;
			if(direction==CC_FACE_COLL_UP)
				r.pixnumber=pixnumber;
			else
				r.pixnumber=pixnumber-ps;
		break;

		case(CC_FXZ):
			r.face_type=CC_VOL;
			if(direction==CC_FACE_COLL_UP)
				r.pixnumber=pixnumber;
			else
				r.pixnumber=pixnumber-rs;
		break;

		case(CC_FYZ):
			r.face_type=CC_VOL;
			if(direction==CC_FACE_COLL_UP)
				r.pixnumber=pixnumber;
			else
				r.pixnumber=pixnumber-1;
		break;

		default:
			fprintf(stderr, "cca_get_container_of_free_face: Error, unknown face type.\n");
			r.face_type=EMPTY;
			r.pixnumber=0;
		break;
	}

	return r;
}



//Input : a face f and a face g
//Output : a face h such that h is the smallest face containing f and g
face_desc cca_get_common_container(struct xvimage* cca_image, unsigned char facetype1, uint32_t pixnumber1, unsigned char facetype2, uint32_t pixnumber2)
{
	face_desc result, t, u;
	uint32_t i, j, rs, ps;

	rs=rowsize(cca_image);
	ps=rs*colsize(cca_image);


	if(facetype1 == EMPTY || facetype2 == EMPTY)
	{
		result.face_type=EMPTY;
		result.pixnumber=0;
		return result;
	}

	result.face_type=EMPTY;
	result.pixnumber=0;

	if(cca_get_face_dimension(facetype1) < cca_get_face_dimension(facetype2))
		return (cca_get_common_container(cca_image, facetype2, pixnumber2, facetype1, pixnumber1));

	else if(cca_get_face_dimension(facetype1) > cca_get_face_dimension(facetype2))
	{
		for(i=0; i<cca_get_number_container(facetype2); i++)
		{
			t=cca_get_one_container(pixnumber2, facetype2, i, getxfrompixnum(pixnumber2, rs, ps), getyfrompixnum(pixnumber2, rs, ps), getzfrompixnum(pixnumber2, rs, ps), cca_image);
			if(t.face_type != EMPTY)
			{
				t=cca_get_common_container(cca_image, facetype1, pixnumber1, t.face_type, t.pixnumber);
				if( (t.face_type != EMPTY) && ((result.face_type==EMPTY) || (cca_get_face_dimension(t.face_type) <= cca_get_face_dimension(result.face_type))) )
				{
					result.pixnumber = t.pixnumber;
					result.face_type = t.face_type;
				}
			}
		}
	}

	else
	{
		if(facetype1 == facetype2)
		{
			if(pixnumber1==pixnumber2)
			{
				result.face_type=facetype1;
				result.pixnumber=pixnumber1;
				return(result);
			}
			else if(facetype1==CC_VOL)
			{
				result.face_type=EMPTY;
				return(result);
			}
		}

		for(i=0; i<cca_get_number_container(facetype1); i++)
		{
			t=cca_get_one_container(pixnumber1, facetype1, i, getxfrompixnum(pixnumber1, rs, ps), getyfrompixnum(pixnumber1, rs, ps), getzfrompixnum(pixnumber1, rs, ps), cca_image);
			if(t.face_type!=EMPTY)
			{
				for(j=0; j<cca_get_number_container(facetype2); j++)
				{
					u=cca_get_one_container(pixnumber2, facetype2, j, getxfrompixnum(pixnumber2, rs, ps), getyfrompixnum(pixnumber2, rs, ps), getzfrompixnum(pixnumber2, rs, ps), cca_image);
					if(u.face_type!=EMPTY)
					{
						u=cca_get_common_container(cca_image, t.face_type, t.pixnumber, u.face_type, u.pixnumber);
						if( (u.face_type != EMPTY) && ((result.face_type==EMPTY) || (cca_get_face_dimension(u.face_type) <= cca_get_face_dimension(result.face_type))) )
						{
							result.pixnumber = u.pixnumber;
							result.face_type = u.face_type;
						}
					}
				}
			}
		}

	}

	return result;


}

//Input : a face g
//Output : dim(g)
uint32_t cca_get_face_dimension(unsigned char facetype)
{
	switch (facetype)
	{
		case(CC_PT):
			return(0);

		case(CC_AX):
		case(CC_AY):
		case(CC_AZ):
			return(1);

		case(CC_FXY):
		case(CC_FYZ):
		case(CC_FXZ):
			return(2);

		case(CC_VOL):
			return(3);

		default:
			fprintf(stderr, "cca_get_number_container: Wrong face type.\n");
			return(0);
	}
}

//Input : a face g
//Output : returns the number fo face f such that dim(f)=dim(g)+1 and g \subset f
uint32_t cca_get_number_container(unsigned char facetype)
{
	switch (facetype)
	{
		case(CC_PT):
			return(6);

		case(CC_AX):
		case(CC_AY):
		case(CC_AZ):
			return(4);

		case(CC_FXY):
		case(CC_FYZ):
		case(CC_FXZ):
			return(2);

		case(CC_VOL):
			return(0);

		default:
			fprintf(stderr, "cca_get_number_container: Wrong face type.\n");
			return(0);
	}
}

//Input : A face g
//Output : A face f such that dim(f)=dim(g)+1 and such that g \subset f (use container_number to scan all possible containers)
face_desc cca_get_one_container(uint32_t pixnumber, unsigned char facetype, uint32_t container_number, uint32_t x, uint32_t y, uint32_t z, struct xvimage* cca_image)
{
	face_desc result;
	uint32_t rs, ps;

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;
	result.pixnumber=0;

	switch(facetype)
	{
		case(CC_PT):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_AX;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_AX;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MIN)==0) result.pixnumber=pixnumber-1;
					else result.face_type=EMPTY;
				break;

				case(2):
					result.face_type=CC_AY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(3):
					result.face_type=CC_AY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MIN)==0) result.pixnumber=pixnumber-rs;
					else result.face_type=EMPTY;
				break;

				case(4):
					result.face_type=CC_AZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(5):
					result.face_type=CC_AZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MIN)==0) result.pixnumber=pixnumber-ps;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_PT face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_AX):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_FXY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_FXY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MIN)==0) result.pixnumber=pixnumber-rs;
					else result.face_type=EMPTY;
				break;

				case(2):
					result.face_type=CC_FXZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(3):
					result.face_type=CC_FXZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MIN)==0) result.pixnumber=pixnumber-ps;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_AX face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_AY):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_FXY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_FXY;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MIN)==0) result.pixnumber=pixnumber-1;
					else result.face_type=EMPTY;
				break;

				case(2):
					result.face_type=CC_FYZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(3):
					result.face_type=CC_FYZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MIN)==0) result.pixnumber=pixnumber-ps;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_AY face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_AZ):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_FXZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_FXZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MIN)==0) result.pixnumber=pixnumber-1;
					else result.face_type=EMPTY;
				break;

				case(2):
					result.face_type=CC_FYZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(3):
					result.face_type=CC_FYZ;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MIN)==0) result.pixnumber=pixnumber-rs;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_AZ face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_FXY):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Z_BORDER_MIN)==0) result.pixnumber=pixnumber-ps;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_FXY face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_FYZ):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & X_BORDER_MIN)==0) result.pixnumber=pixnumber-1;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_FYZ face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		case(CC_FXZ):
			switch(container_number)
			{
				case(0):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MAX)==0) result.pixnumber=pixnumber;
					else result.face_type=EMPTY;
				break;

				case(1):
					result.face_type=CC_VOL;
					if( (cca_is_face_onborder(cca_image, facetype, x, y, z) & Y_BORDER_MIN)==0) result.pixnumber=pixnumber-rs;
					else result.face_type=EMPTY;
				break;

				default:
					fprintf(stderr, "cca_get_one_container: Wrong container_number given for CC_FXZ face.\n");
					result.face_type=EMPTY;
				break;
			}
		break;

		default:
			fprintf(stderr, "cca_get_one_container: Wrong facetype given.\n");
			result.face_type=EMPTY;
		break;
	}

	return result;
}



uint32_t cca_getfacedist(struct xvimage *cca_image, struct xvimage *distmap, uint32_t i, uint32_t j, uint32_t k, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	uint32_t pix, bord, newps, newrs, max;

	bord=cca_is_face_onborder(cca_image, face_type, i, j, k);

	newps = (rowsize(cca_image)-1)*(colsize(cca_image)-1);
	newrs=rowsize(cca_image)-1;

	pix=i+j*(newrs)+k*(newps);
	switch(face_type)
	{
		case(CC_FXY):
			if( (bord & Z_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-newps])
					return(ULONGDATA(distmap)[pix]);
				else
					return(ULONGDATA(distmap)[pix-newps]);
			}
			else
				return(ULONGDATA(distmap)[pix]);
		break;

		case(CC_FYZ):
			if( (bord & X_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-1])
					return(ULONGDATA(distmap)[pix]);
				else
					return(ULONGDATA(distmap)[pix-1]);
			}
			else
				return(ULONGDATA(distmap)[pix]);
		break;

		case(CC_FXZ):
			if( (bord & Y_BORDER_MIN) == 0 )
			{
				if(ULONGDATA(distmap)[pix]>=ULONGDATA(distmap)[pix-newrs])
					return(ULONGDATA(distmap)[pix]);
				else
					return(ULONGDATA(distmap)[pix-newrs]);
			}
			else
				return (ULONGDATA(distmap)[pix]);
		break;

		case(CC_AX):
			max=ULONGDATA(distmap)[pix];
			if( ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs]) ) max=ULONGDATA(distmap)[pix-newrs];
			if( ((bord & Z_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newps]) ) max=ULONGDATA(distmap)[pix-newps];
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs-newps]) ) max=ULONGDATA(distmap)[pix-newrs-newps];

			return(max);

		break;

		case(CC_AY):
			max=ULONGDATA(distmap)[pix];
			if( ((bord & X_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-1]) ) max=ULONGDATA(distmap)[pix-1];
			if( ((bord & Z_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newps]) ) max=ULONGDATA(distmap)[pix-newps];
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-1-newps]) ) max=ULONGDATA(distmap)[pix-1-newps];

			return(max);

		break;

		case(CC_AZ):
			max=ULONGDATA(distmap)[pix];
			if( ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs]) ) max=ULONGDATA(distmap)[pix-newrs];
			if( ((bord & X_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-1]) ) max=ULONGDATA(distmap)[pix-1];
			if( ((bord & X_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs-1]) ) max=ULONGDATA(distmap)[pix-newrs-1];

			return(max);

		break;

		case(CC_PT):
			max=ULONGDATA(distmap)[pix];
			if( ((bord & Z_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newps]) ) max=ULONGDATA(distmap)[pix-newps];
			if( ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs]) ) max=ULONGDATA(distmap)[pix-newrs];
			if( ((bord & X_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-1]) ) max=ULONGDATA(distmap)[pix-1];
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs-newps]) ) max=ULONGDATA(distmap)[pix-newrs-newps];
			if( ((bord & Z_BORDER_MIN) == 0) && ((bord & X_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-1-newps]) ) max=ULONGDATA(distmap)[pix-1-newps];
			if( ((bord & X_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs-1]) ) max=ULONGDATA(distmap)[pix-newrs-1];
			if( ((bord & X_BORDER_MIN) == 0) && ((bord & Y_BORDER_MIN) == 0) && ((bord & Z_BORDER_MIN) == 0) && (max < ULONGDATA(distmap)[pix-newrs-1]) ) max=ULONGDATA(distmap)[pix-newps-newrs-1];
			return(max);

		break;

		default:
			fprintf(stderr, "cca_getfacedist(): Wrong face type given.\n");
	}

	return(0);
}


void cca_add_complexface(struct xvimage *image, unsigned char face_type, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	switch(face_type)
	{
		case (CC_VOL):
			//We have a cube... We must first add eight vertex, twelve edges, six faces and a volume
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_VOL | (unsigned char)CC_FXY | (unsigned char)CC_FXZ | (unsigned char)CC_FYZ | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] |= ((unsigned char)CC_FYZ | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] |= ((unsigned char)CC_FXZ | (unsigned char)CC_AX | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs] |= ((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] |= ((unsigned char)CC_FXY | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+ps] |= ((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs+ps] |= ((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs+ps] |= (unsigned char)CC_PT;
			break;

		case (CC_FXY):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_FXY | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] |= ((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] |= ((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs] |= (unsigned char)CC_PT;
			break;

		case (CC_FXZ):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_FXZ | (unsigned char)CC_AX | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] |= ((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] |= ((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+ps] |= (unsigned char)CC_PT;
			break;

		case (CC_FYZ):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_FYZ | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] |= ((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] |= ((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs+ps] |= (unsigned char)CC_PT;
			break;

		case (CC_AX):
			//We have an edhe, so we add the two vertices and the edge
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] |= (unsigned char)CC_PT;
			break;

		case (CC_AY):
			//We have a face, so we first add the four edges and four vertices
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] |= (unsigned char)CC_PT;
			break;

		case (CC_AZ):
			//We have a face, so we first add the four edges and four vertices
			UCHARDATA(image)[pixnumber] |= ((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] |= (unsigned char)CC_PT;
			break;

		case(CC_PT):
			//We add the vertex
			UCHARDATA(image)[pixnumber] |= (unsigned char)CC_PT;
			break;

		default:
			fprintf(stderr, "cca_add_complexface(): Error, unrecognised face type.\n");
	}
}


void cca_remove_complexface(struct xvimage *image, unsigned char face_type, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	switch(face_type)
	{
		case (CC_VOL):
			//We have a cube... We must first add eight vertex, twelve edges, six faces and a volume
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_VOL | (unsigned char)CC_FXY | (unsigned char)CC_FXZ | (unsigned char)CC_FYZ | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] &= 255-((unsigned char)CC_FYZ | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] &= 255-((unsigned char)CC_FXZ | (unsigned char)CC_AX | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs] &= 255-((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] &= 255-((unsigned char)CC_FXY | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+ps] &= 255-((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs+ps] &= 255-((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs+ps] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_FXY):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_FXY | (unsigned char)CC_AX | (unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] &= 255-((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] &= 255-((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+rs] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_FXZ):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_FXZ | (unsigned char)CC_AX | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] &= 255-((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] &= 255-((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1+ps] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_FYZ):
			//We have a face, so we first add the face, the four edges and four vertices
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_FYZ | (unsigned char)CC_AY | (unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] &= 255-((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] &= 255-((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs+ps] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_AX):
			//We have an edhe, so we add the two vertices and the edge
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_AX | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+1] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_AY):
			//We have a face, so we first add the four edges and four vertices
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_AY | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+rs] &= 255-(unsigned char)CC_PT;
			break;

		case (CC_AZ):
			//We have a face, so we first add the four edges and four vertices
			UCHARDATA(image)[pixnumber] &= 255-((unsigned char)CC_AZ | (unsigned char)CC_PT);
			UCHARDATA(image)[pixnumber+ps] &= 255-(unsigned char)CC_PT;
			break;

		case(CC_PT):
			//We add the vertex
			UCHARDATA(image)[pixnumber] &= 255-(unsigned char)CC_PT;
			break;

		default:
			fprintf(stderr, "cca_remove_complexface(): Error, unrecognised face type.\n");
	}
}




/*!	\brief Is the face on the border of the image?

	\param cca_image The cubical complex containing the face to test.
	\param face_type Which face to test (CC_FXY, CC_FXZ, CC_FYZ, CC_AX, CC_AY, CC_AZ, CC_PT). A volume is never on the border.
	\param coord_x The x coordinate of the voxel containing the face.
	\param coord_y The y coordinate of the voxel containing the face.
	\param coord_z The z coordinate of the voxel containing the face.

	\ingroup complex

	\return Code depending on which border the face is (see libcomplex.h for more details).

	\author John Chaussard
*/
uint32_t cca_is_face_onborder(struct xvimage *cca_image, int32_t face_type, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z)
{
	switch(face_type)
	{
		case (CC_FXY):
			return( ((coord_z == 0) << 4) | ((coord_z == (uint32_t)depth(cca_image)-1) << 5) );
			break;

		case (CC_FXZ):
			return( ((coord_y == 0) << 2) | ((coord_y == (uint32_t)colsize(cca_image)-1) << 3) );
			break;

		case (CC_FYZ):
			return( (coord_x == 0) | ((coord_x == (uint32_t)rowsize(cca_image)-1) << 1) );
			break;

		case (CC_AX):
			return( ((coord_z == 0) << 4) | ((coord_z == (uint32_t)depth(cca_image)-1) << 5) | ((coord_y == 0) << 2) | ((coord_y == (uint32_t)colsize(cca_image)-1) << 3) );
			break;

		case (CC_AY):
			return( ((coord_z == 0) << 4) | ((coord_z == (uint32_t)depth(cca_image)-1) << 5) | (coord_x == 0) | ((coord_x == (uint32_t)rowsize(cca_image)-1) << 1) );
			break;

		case (CC_AZ):
			return( ((coord_y == 0) << 2) | ((coord_y == (uint32_t)colsize(cca_image)-1) << 3) | (coord_x == 0) | ((coord_x == (uint32_t)rowsize(cca_image)-1) << 1) );
			break;

		case (CC_PT):
			return( ((coord_z == 0) << 4) | ((coord_z == (uint32_t)depth(cca_image)-1) << 5) | ((coord_y == 0) << 2) | ((coord_y == (uint32_t)colsize(cca_image)-1) << 3) | (coord_x == 0) | ((coord_x == (uint32_t)rowsize(cca_image)-1) << 1) );
			break;

		default:
			fprintf(stderr, "cca_is_face_onborder(): Unknown configuration given.\n");
			return(0);
	}

	fprintf(stderr, "cca_is_face_onborder(): Unknown configuration given (3).\n");
	return(0);
}


uint32_t cca_is_face_onborder_v2(int32_t face_type, uint32_t pixnumber, uint32_t rs, uint32_t cs, uint32_t d)
{
	switch(face_type)
	{
		case(CC_VOL):
			return EMPTY;
			break;

		case (CC_FXY):
			return( ((getzfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 4) | ((getzfrompixnum(pixnumber, rs, (rs*cs)) == d-1) << 5) );
			break;

		case (CC_FXZ):
			return( ((getyfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 2) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == cs-1) << 3) );
			break;

		case (CC_FYZ):
			return( (getxfrompixnum(pixnumber, rs, (rs*cs)) == 0) | ((getxfrompixnum(pixnumber, rs, (rs*cs)) == rs-1) << 1) );
			break;

		case (CC_AX):
			return( ((getzfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 4) | ((getzfrompixnum(pixnumber, rs, (rs*cs)) == d-1) << 5) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 2) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == cs-1) << 3) );
			break;

		case (CC_AY):
			return( ((getzfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 4) | ((getzfrompixnum(pixnumber, rs, (rs*cs)) == d-1) << 5) | (getxfrompixnum(pixnumber, rs, (rs*cs)) == 0) | ((getxfrompixnum(pixnumber, rs, (rs*cs)) == rs-1) << 1) );
			break;

		case (CC_AZ):
			return( ((getyfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 2) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == cs-1) << 3) | (getxfrompixnum(pixnumber, rs, (rs*cs)) == 0) | ((getxfrompixnum(pixnumber, rs, (rs*cs)) == rs-1) << 1) );
			break;

		case (CC_PT):
			return( ((getzfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 4) | ((getzfrompixnum(pixnumber, rs, (rs*cs)) == d-1) << 5) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == 0) << 2) | ((getyfrompixnum(pixnumber, rs, (rs*cs)) == cs-1) << 3) | (getxfrompixnum(pixnumber, rs, (rs*cs)) == 0) | ((getxfrompixnum(pixnumber, rs, (rs*cs)) == rs-1) << 1) );
			break;

		default:
			fprintf(stderr, "cca_is_face_onborder_v2(): Unknown configuration given.\n");
			return(0);
	}
	fprintf(stderr, "cca_is_face_onborder_v2(): Unknown configuration given.\n");
	return(0);
}


void cca_keep_only_cells(struct xvimage* image)
{
	int32_t i, N, rs, ps;

	rs=rowsize(image);
	ps=rs*colsize(image);
	N=rowsize(image)*colsize(image)*depth(image);

	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i]&CC_VOL) != 0)
		{
			UCHARDATA(image)[i] &= CC_VOL; //On retire tous les autres éléments que le volume...
			UCHARDATA(image)[i+1] &= (255-CC_FYZ-CC_AY-CC_AZ-CC_PT); //On retire tous les autres éléments que le volume...
			UCHARDATA(image)[i+rs] &= (255-CC_FXZ-CC_AX-CC_AZ-CC_PT);
			UCHARDATA(image)[i+ps] &= (255-CC_FXY-CC_AX-CC_AY-CC_PT);
			UCHARDATA(image)[i+rs+1] &= (255-CC_AZ-CC_PT);
			UCHARDATA(image)[i+ps+1] &= (255-CC_AY-CC_PT);
			UCHARDATA(image)[i+ps+rs] &= (255-CC_AX-CC_PT);
			UCHARDATA(image)[i+ps+rs+1] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_FXY) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_AY-CC_AX-CC_PT);
			UCHARDATA(image)[i+1] &= (255-CC_AY-CC_PT);
			UCHARDATA(image)[i+rs] &= (255-CC_AX-CC_PT);
			UCHARDATA(image)[i+1+rs] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_FXZ) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_AZ-CC_AX-CC_PT);
			UCHARDATA(image)[i+1] &= (255-CC_AZ-CC_PT);
			UCHARDATA(image)[i+ps] &= (255-CC_AX-CC_PT);
			UCHARDATA(image)[i+1+ps] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_FYZ) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_AY-CC_AZ-CC_PT);
			UCHARDATA(image)[i+ps] &= (255-CC_AY-CC_PT);
			UCHARDATA(image)[i+rs] &= (255-CC_AZ-CC_PT);
			UCHARDATA(image)[i+ps+rs] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_AX) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_PT);
			UCHARDATA(image)[i+1] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_AY) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_PT);
			UCHARDATA(image)[i+rs] &= (255-CC_PT);
		}

		if( (UCHARDATA(image)[i]&CC_AZ) != 0)
		{
			UCHARDATA(image)[i] &= (255-CC_PT);
			UCHARDATA(image)[i+ps] &= (255-CC_PT);
		}
	}
}

void cca_makecomplex(struct xvimage *cca_image)
{
	uint32_t i, N, rs, ps;

	rs=rowsize(cca_image);
	ps=colsize(cca_image)*rs;
	N=ps*depth(cca_image);

	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(cca_image)[i] & CC_PT) != 0)
			cca_add_complexface(cca_image, CC_PT, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_AX) != 0)
			cca_add_complexface(cca_image, CC_AX, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_AY) != 0)
			cca_add_complexface(cca_image, CC_AY, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_AY) != 0)
			cca_add_complexface(cca_image, CC_AY, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_AZ) != 0)
			cca_add_complexface(cca_image, CC_AZ, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_FXY) != 0)
			cca_add_complexface(cca_image, CC_FXY, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_FYZ) != 0)
			cca_add_complexface(cca_image, CC_FYZ, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_FXZ) != 0)
			cca_add_complexface(cca_image, CC_FXZ, i, rs, ps);

		if( (UCHARDATA(cca_image)[i] & CC_VOL) != 0)
			cca_add_complexface(cca_image, CC_VOL, i, rs, ps);
	}
}

uint32_t cca_cardinal_containers(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	int32_t result=0;

	switch(face_type)
	{
		case(CC_PT):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_AX)!=0)) result++;
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_AY)!=0)) result++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_AZ)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_AX)!=0) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_AY)!=0) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_AZ)!=0) result++;
			break;

		case(CC_AX):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FXY)!=0)) result++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FXZ)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0) result++;
			break;

		case(CC_AY):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXY)!=0)) result++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FYZ)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0) result++;
			break;

		case(CC_AZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXZ)!=0)) result++;
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FYZ)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0) result++;
			break;

		case(CC_FXY):
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		case(CC_FXZ):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		case(CC_FYZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		default:
			fprintf(stderr, "cca_cardinal_containers() : Given face_type is incorrect or not yet implemented.\n");
			return(0);
	}
	return(result);
}


void cca_get_subface(unsigned char facetype, uint32_t pixnumber, uint32_t subface_index, face_desc *result, uint32_t rs, uint32_t ps)
{
	switch(facetype)
	{
		case(CC_VOL):
			//Subface 0 to 1 are CC_FXY, 2 to 3 are CC_FXZ, 4 to 5 are CC_FYZ
			//6 to 9 are CC_AX, 10 to 13 are CC_AY, 14 to 17 are CC_AZ
			//18 to 25 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_FXY;break;
				case(1): result->pixnumber=pixnumber+ps; result->face_type=CC_FXY;break;
				case(2): result->pixnumber=pixnumber; result->face_type=CC_FXZ;break;
				case(3): result->pixnumber=pixnumber+rs; result->face_type=CC_FXZ;break;
				case(4): result->pixnumber=pixnumber; result->face_type=CC_FYZ;break;
				case(5): result->pixnumber=pixnumber+1; result->face_type=CC_FYZ;break;
				case(6): result->pixnumber=pixnumber; result->face_type=CC_AX;break;
				case(7): result->pixnumber=pixnumber+rs; result->face_type=CC_AX;break;
				case(8): result->pixnumber=pixnumber+ps; result->face_type=CC_AX;break;
				case(9): result->pixnumber=pixnumber+rs+ps; result->face_type=CC_AX;break;
				case(10): result->pixnumber=pixnumber; result->face_type=CC_AY;break;
				case(11): result->pixnumber=pixnumber+1; result->face_type=CC_AY;break;
				case(12): result->pixnumber=pixnumber+ps; result->face_type=CC_AY;break;
				case(13): result->pixnumber=pixnumber+1+ps; result->face_type=CC_AY;break;
				case(14): result->pixnumber=pixnumber; result->face_type=CC_AZ;break;
				case(15): result->pixnumber=pixnumber+1; result->face_type=CC_AZ;break;
				case(16): result->pixnumber=pixnumber+rs; result->face_type=CC_AZ;break;
				case(17): result->pixnumber=pixnumber+1+rs; result->face_type=CC_AZ;break;
				case(18): result->pixnumber=pixnumber; result->face_type=CC_PT;break;
				case(19): result->pixnumber=pixnumber+1; result->face_type=CC_PT;break;
				case(20): result->pixnumber=pixnumber+rs; result->face_type=CC_PT;break;
				case(21): result->pixnumber=pixnumber+ps; result->face_type=CC_PT;break;
				case(22): result->pixnumber=pixnumber+1+rs; result->face_type=CC_PT;break;
				case(23): result->pixnumber=pixnumber+1+ps; result->face_type=CC_PT;break;
				case(24): result->pixnumber=pixnumber+rs+ps; result->face_type=CC_PT;break;
				case(25): result->pixnumber=pixnumber+1+rs+ps; result->face_type=CC_PT;break;
				default:  fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_FXY):
			//Subface 0 to 1 are CC_AX, 2 to 3 are CC_AY, 4 to 7 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_AX; break;
				case(1): result->pixnumber=pixnumber+rs; result->face_type=CC_AX; break;
				case(2): result->pixnumber=pixnumber; result->face_type=CC_AY; break;
				case(3): result->pixnumber=pixnumber+1; result->face_type=CC_AY; break;
				case(4): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(5): result->pixnumber=pixnumber+rs; result->face_type=CC_PT; break;
				case(6): result->pixnumber=pixnumber+1; result->face_type=CC_PT; break;
				case(7): result->pixnumber=pixnumber+1+rs; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_FXZ):
			//Subface 0 to 1 are CC_AX, 2 to 3 are CC_AZ, 4 to 7 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_AX; break;
				case(1): result->pixnumber=pixnumber+ps; result->face_type=CC_AX; break;
				case(2): result->pixnumber=pixnumber; result->face_type=CC_AZ; break;
				case(3): result->pixnumber=pixnumber+1; result->face_type=CC_AZ; break;
				case(4): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(5): result->pixnumber=pixnumber+ps; result->face_type=CC_PT; break;
				case(6): result->pixnumber=pixnumber+1; result->face_type=CC_PT; break;
				case(7): result->pixnumber=pixnumber+1+ps; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_FYZ):
			//Subface 0 to 1 are CC_AY, 2 to 3 are CC_AZ, 4 to 7 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_AY; break;
				case(1): result->pixnumber=pixnumber+ps; result->face_type=CC_AY; break;
				case(2): result->pixnumber=pixnumber; result->face_type=CC_AZ; break;
				case(3): result->pixnumber=pixnumber+rs; result->face_type=CC_AZ; break;
				case(4): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(5): result->pixnumber=pixnumber+rs; result->face_type=CC_PT; break;
				case(6): result->pixnumber=pixnumber+ps; result->face_type=CC_PT; break;
				case(7): result->pixnumber=pixnumber+ps+rs; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_AX):
			//Subface 0 to 1 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(1): result->pixnumber=pixnumber+1; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_AY):
			//Subface 0 to 1 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(1): result->pixnumber=pixnumber+rs; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		case(CC_AZ):
			//Subface 0 to 1 are CC_PT
			switch(subface_index)
			{
				case(0): result->pixnumber=pixnumber; result->face_type=CC_PT; break;
				case(1): result->pixnumber=pixnumber+ps; result->face_type=CC_PT; break;
				default: fprintf(stderr, "cca_get_subface_pixnumber(): Wrong subface position given.\n"); break;
			}
			break;

		default:
			fprintf(stderr, "cca_get_subface_pixnumber(): Wrong face type given.\n");
			break;
	}
}


//Don't modify indexes... some can be added, but existing ones must not be changed
void cca_get_starface(unsigned char facetype, uint32_t pixnumber, uint32_t starface_index, face_desc *result, uint32_t rs, uint32_t ps, uint32_t border_code)
{
	switch(facetype)
	{
		//Index 0 to 1 is CC_AX, 2 to 3 is CC_AY, 4 to 5 is CC_AZ
		//6 to 9 is CC_FXY, 10 to 13 is CC_FXZ, 14 to 17 is CC_FYZ
		//18 to 25 is CC_VOL
		case(CC_PT):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_AX;
					if( (border_code & X_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_AX;
					if( (border_code & X_BORDER_MIN) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(2):
					result->face_type=CC_AY;
					if( (border_code & Y_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(3):
					result->face_type=CC_AY;
					if( (border_code & Y_BORDER_MIN) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(4):
					result->face_type=CC_AZ;
					if( (border_code & Z_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(5):
					result->face_type=CC_AZ;
					if( (border_code & Z_BORDER_MIN) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(6):
					result->face_type=CC_FXY;
					if( (border_code & (X_BORDER_MAX | Y_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(7):
					result->face_type=CC_FXY;
					if( (border_code & (X_BORDER_MIN | Y_BORDER_MAX)) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(8):
					result->face_type=CC_FXY;
					if( (border_code & (X_BORDER_MAX | Y_BORDER_MIN)) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(9):
					result->face_type=CC_FXY;
					if( (border_code & (X_BORDER_MIN | Y_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-rs;
					else result->face_type=EMPTY;
					break;

				case(10):
					result->face_type=CC_FXZ;
					if( (border_code & (X_BORDER_MAX | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(11):
					result->face_type=CC_FXZ;
					if( (border_code & (X_BORDER_MIN | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(12):
					result->face_type=CC_FXZ;
					if( (border_code & (X_BORDER_MAX | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(13):
					result->face_type=CC_FXZ;
					if( (border_code & (X_BORDER_MIN | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-ps;
					else result->face_type=EMPTY;
					break;

				case(14):
					result->face_type=CC_FYZ;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(15):
					result->face_type=CC_FYZ;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(16):
					result->face_type=CC_FYZ;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(17):
					result->face_type=CC_FYZ;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-rs-ps;
					else result->face_type=EMPTY;
					break;

				case(18):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MAX | X_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(19):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MAX | X_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(20):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MAX | X_BORDER_MAX)) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(21):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MIN | X_BORDER_MAX)) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(22):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MAX | X_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-rs;
					else result->face_type=EMPTY;
					break;

				case(23):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MIN | X_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-ps;
					else result->face_type=EMPTY;
					break;

				case(24):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MIN | X_BORDER_MAX)) ==0) result->pixnumber=pixnumber-rs-ps;
					else result->face_type=EMPTY;
					break;

				case(25):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MIN | X_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-rs-ps;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_FXY, 2 to 3 is CC_FXZ, 4 to 8 is CC_VOL
		case(CC_AX):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_FXY;
					if( (border_code & Y_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_FXY;
					if( (border_code & Y_BORDER_MIN) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(2):
					result->face_type=CC_FXZ;
					if( (border_code & Z_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(3):
					result->face_type=CC_FXZ;
					if( (border_code & Z_BORDER_MIN) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(4):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(5):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(6):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MAX | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(7):
					result->face_type=CC_VOL;
					if( (border_code & (Y_BORDER_MIN | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-rs-ps;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_FXY, 2 to 3 is CC_FYZ, 4 to 8 is CC_VOL
		case(CC_AY):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_FXY;
					if( (border_code & X_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_FXY;
					if( (border_code & X_BORDER_MIN) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(2):
					result->face_type=CC_FYZ;
					if( (border_code & Z_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(3):
					result->face_type=CC_FYZ;
					if( (border_code & Z_BORDER_MIN) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(4):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MAX | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(5):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MIN | Z_BORDER_MAX)) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(6):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MAX | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				case(7):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MIN | Z_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-ps;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_FXZ, 2 to 3 is CC_FYZ, 4 to 8 is CC_VOL
		case(CC_AZ):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_FXZ;
					if( (border_code & X_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_FXZ;
					if( (border_code & X_BORDER_MIN) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(2):
					result->face_type=CC_FYZ;
					if( (border_code & Y_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(3):
					result->face_type=CC_FYZ;
					if( (border_code & Y_BORDER_MIN) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(4):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MAX | Y_BORDER_MAX)) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(5):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MIN | Y_BORDER_MAX)) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				case(6):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MAX | Y_BORDER_MIN)) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				case(7):
					result->face_type=CC_VOL;
					if( (border_code & (X_BORDER_MIN | Y_BORDER_MIN)) ==0) result->pixnumber=pixnumber-1-rs;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_VOL
		case(CC_FXY):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_VOL;
					if( (border_code & Z_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_VOL;
					if( (border_code & Z_BORDER_MIN) ==0) result->pixnumber=pixnumber-ps;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_VOL
		case(CC_FXZ):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_VOL;
					if( (border_code & Y_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_VOL;
					if( (border_code & Y_BORDER_MIN) ==0) result->pixnumber=pixnumber-rs;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		//Index 0 to 1 is CC_VOL
		case(CC_FYZ):
			switch(starface_index)
			{
				case(0):
					result->face_type=CC_VOL;
					if( (border_code & X_BORDER_MAX) ==0) result->pixnumber=pixnumber;
					else result->face_type=EMPTY;
					break;

				case(1):
					result->face_type=CC_VOL;
					if( (border_code & X_BORDER_MIN) ==0) result->pixnumber=pixnumber-1;
					else result->face_type=EMPTY;
					break;

				default:
					fprintf(stderr, "cca_get_starface(): Wrong starface position given.\n");
					break;
			}
			break;

		default:
			fprintf(stderr, "cca_get_subface_pixnumber(): Wrong face type given.\n");
			break;
	}
}


uint32_t cca_get_cardinal_subface(unsigned char facetype)
{
	switch(facetype)
	{
		case(CC_VOL): return 26; break;
		case(CC_FXY):
		case(CC_FXZ):
		case(CC_FYZ): return 8; break;
		case(CC_AX):
		case(CC_AY):
		case(CC_AZ): return 2; break;
		case(CC_PT): return 0; break;
		default: fprintf(stderr, "cca_get_cardinal_pixnumber(): Wrong face type given.\n"); break;
	}
	return(0);
}


uint32_t cca_get_cardinal_starface(unsigned char facetype)
{
	switch(facetype)
	{
		case(CC_VOL): return 0; break;
		case(CC_FXY):
		case(CC_FXZ):
		case(CC_FYZ): return 2; break;
		case(CC_AX):
		case(CC_AY):
		case(CC_AZ): return 8; break;
		case(CC_PT): return 26; break;
		default: fprintf(stderr, "cca_get_cardinal_pixnumber(): Wrong face type given.\n"); break;
	}
	return(0);
}


//In order to have sense, image should be only made of cells (no subfaces)
//An intersection face is contained in more than two elements of same dimension OR in at least two elements of different dimension
uint32_t cca_face_is_an_intersection_face(struct xvimage *image, unsigned char facetype, uint32_t pixnumber, uint32_t rs, uint32_t ps, uint32_t border_code)
{
	uint32_t result[3];
	uint32_t num_vol, num_face, num_edge, n, i;
	face_desc f;
	num_vol=0; num_face=0; num_edge=0;
	n=cca_get_cardinal_starface(facetype);

	result[0]=0; result[1]=0; result[2]=0;

	for(i=0; i<n; i++)
	{
		cca_get_starface(facetype, pixnumber, i, &f, rs, ps, border_code);
		if( (f.face_type!=EMPTY) && ((UCHARDATA(image)[f.pixnumber] & f.face_type)!=0) )
			result[cca_get_face_dimension(f.face_type) -1]++;
	}

	return( !(	((result[2]==0) && (result[1]==0) && (result[0]<=2)) ||
				((result[2]==0) && (result[1]<=2) && (result[0]==0)) ||
				((result[2]<=2) && (result[1]==0) && (result[0]==0)) ));
}


//List must be a DYNAMIC list of face_desc
//Image is changed by the function (elements added to the list are removed from the image
void cca_move_subfaces_to_list(struct xvimage *image, list *l, face_desc *fd, uint32_t rs, uint32_t ps)
{
	face_desc f;
	uint32_t i, n;

	n=cca_get_cardinal_subface(fd->face_type);
	for(i=0; i<n; i++)
	{
		cca_get_subface(fd->face_type, fd->pixnumber, i, &f, rs, ps);
		if( (UCHARDATA(image)[f.pixnumber] & f.face_type) != 0)
		{
			list_push_pointer(l, &f);
			UCHARDATA(image)[f.pixnumber] = UCHARDATA(image)[f.pixnumber] - f.face_type;
		}
	}
}


//List must be a DYNAMIC list of face_desc
//Image is changed by the function (elements added to the list are removed from the image
void cca_move_starfaces_to_list(struct xvimage *image, list *l, face_desc *fd, uint32_t rs, uint32_t ps)
{
	face_desc f;
	uint32_t i, n;
	unsigned char border_code=cca_is_face_onborder_v2(fd->face_type, fd->pixnumber, rs, colsize(image), depth(image));

	n=cca_get_cardinal_starface(fd->face_type);
	for(i=0; i<n; i++)
	{
		cca_get_starface(fd->face_type, fd->pixnumber, i, &f, rs, ps, border_code);
		if( (f.face_type!=EMPTY) && ((UCHARDATA(image)[f.pixnumber] & f.face_type) != 0)) //If face_type is empty, then test will be false, so it's good...
		{
			list_push_pointer(l, &f);
			UCHARDATA(image)[f.pixnumber] = UCHARDATA(image)[f.pixnumber] - f.face_type;
		}
	}
}


//List must be a DYNAMIC list of face_desc
/*void cca_add_to_list_starface_dimension_direct_over(struct xvimage* image, list *l, face_desc *fd, uint32_t rs, uint32_t cs, uint32_t d)
{
	face_desc f;
	uint32_t bord;
	uint32_t pixnumber=fd->pixnumber;

	bord=cca_is_face_onborder_v2(face_type, pixnumber, rs, cs, d);


	switch(face_type)
	{
		case(CC_VOL):
			break;

		case(CC_FXY):
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-rs*cs; list_push(l, &f);}
			break;

		case(CC_FXZ):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-rs; list_push(l, &f);}
			break;

		case(CC_FYZ):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-1; list_push(l, &f);}
			break;

		case(CC_AX):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber-rs; list_push(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber-rs*cs; list_push(l, &f);}
			break;

		case(CC_AY):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber-1; list_push(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber-rs*cs; list_push(l, &f);}
			break;

		case(CC_AZ):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber-rs; list_push(l, &f);}
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber-1; list_push(l, &f);}
			break;

		case(CC_PT):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_AX; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_AX; f.pixnumber=pixnumber-1; list_push(l, &f);}
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_AY; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_AY; f.pixnumber=pixnumber-rs; list_push(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_AZ; f.pixnumber=pixnumber; list_push(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_AZ; f.pixnumber=pixnumber-rs*cs; list_push(l, &f);}
			break;

		default:
			fprintf(stderr, "cca_list_starface_dimension_direct_over(): Face type invalid (%ud).\n", face_type);
			return(NULL);
	}
}*/

list *cca_list_subfaces_dimension_direct_under(uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	face_desc f;

	list *l=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 6);
	if(l==NULL)
	{
		fprintf(stderr, "cca_list_subfaces_dimension_direct_under(): Could not create list.\n");
		return(NULL);
	}

	switch(face_type)
	{
		case(CC_VOL):
			f.face_type = CC_FXY; f.pixnumber=pixnumber;list_push_pointer(l, &f);
			f.pixnumber=pixnumber+ps; list_push_pointer(l, &f);
			f.face_type = CC_FXZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+rs; list_push_pointer(l, &f);
			f.face_type = CC_FYZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+1; list_push_pointer(l, &f);
			break;

		case(CC_FXY):
			f.face_type = CC_AX; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+rs; list_push_pointer(l, &f);
			f.face_type = CC_AY; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+1; list_push_pointer(l, &f);
			break;

		case(CC_FXZ):
			f.face_type = CC_AX; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+ps; list_push_pointer(l, &f);
			f.face_type = CC_AZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+1; list_push_pointer(l, &f);
			break;

		case(CC_FYZ):
			f.face_type = CC_AY; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+ps; list_push_pointer(l, &f);
			f.face_type = CC_AZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+rs; list_push_pointer(l, &f);
			break;

		case(CC_AX):
			f.face_type = CC_PT; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+1; list_push_pointer(l, &f);
			break;

		case(CC_AY):
			f.face_type = CC_PT; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+rs; list_push_pointer(l, &f);
			break;

		case(CC_AZ):
			f.face_type = CC_PT; f.pixnumber=pixnumber; list_push_pointer(l, &f);
			f.pixnumber=pixnumber+ps; list_push_pointer(l, &f);
			break;

		case(CC_PT):
			break;

		default:
			fprintf(stderr, "cca_list_subfaces_dimension_direct_under(): Face type invalid (%ud).\n", face_type);
			return(NULL);
	}

	return l;
}


//starface == face which contains
list *cca_list_starface_dimension_direct_over(uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t cs, uint32_t d)
{
	face_desc f;
	uint32_t bord;

	list *l=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 6);
	if(l==NULL)
	{
		fprintf(stderr, "cca_list_starface_dimension_direct_over(): Could not create list.\n");
		return(NULL);
	}


	bord=cca_is_face_onborder_v2(face_type, pixnumber, rs, cs, d);


	switch(face_type)
	{
		case(CC_VOL):
			break;

		case(CC_FXY):
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-rs*cs; list_push_pointer(l, &f);}
			break;

		case(CC_FXZ):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-rs; list_push_pointer(l, &f);}
			break;

		case(CC_FYZ):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_VOL; f.pixnumber=pixnumber-1; list_push_pointer(l, &f);}
			break;

		case(CC_AX):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber-rs; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber-rs*cs; list_push_pointer(l, &f);}
			break;

		case(CC_AY):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_FXY; f.pixnumber=pixnumber-1; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber-rs*cs; list_push_pointer(l, &f);}
			break;

		case(CC_AZ):
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_FYZ; f.pixnumber=pixnumber-rs; list_push_pointer(l, &f);}
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_FXZ; f.pixnumber=pixnumber-1; list_push_pointer(l, &f);}
			break;

		case(CC_PT):
			if((bord & X_BORDER_MAX)==0){f.face_type = CC_AX; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & X_BORDER_MIN)==0){f.face_type = CC_AX; f.pixnumber=pixnumber-1; list_push_pointer(l, &f);}
			if((bord & Y_BORDER_MAX)==0){f.face_type = CC_AY; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Y_BORDER_MIN)==0){f.face_type = CC_AY; f.pixnumber=pixnumber-rs; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MAX)==0){f.face_type = CC_AZ; f.pixnumber=pixnumber; list_push_pointer(l, &f);}
			if((bord & Z_BORDER_MIN)==0){f.face_type = CC_AZ; f.pixnumber=pixnumber-rs*cs; list_push_pointer(l, &f);}
			break;

		default:
			fprintf(stderr, "cca_list_starface_dimension_direct_over(): Face type invalid (%ud).\n", face_type);
			return(NULL);
	}

	return l;
}


int32_t cca_is_face_inside_surface(struct xvimage* cca_image, uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	list *l;
	face_desc *f;
	uint32_t inside_surface=1;

	l=cca_list_subfaces_dimension_direct_under(pixnumber, face_type, rs, ps);

	while(!list_isempty(l))
	{
		f=(face_desc*)list_pop_pointer(l);
		if(	inside_surface==1 &&
			cca_count_cube_container(cca_image, f->pixnumber, getxfrompixnum(f->pixnumber, rs, ps), getyfrompixnum(f->pixnumber, rs, ps), getzfrompixnum(f->pixnumber, rs, ps), f->face_type, rs, ps)!=0 &&
			cca_cardinal_containers(cca_image, f->pixnumber, getxfrompixnum(f->pixnumber, rs, ps), getyfrompixnum(f->pixnumber, rs, ps), getzfrompixnum(f->pixnumber, rs, ps), f->face_type, rs, ps)!=2 )
		{
			inside_surface=0;
		}
	}

	list_delete(l, NO_FREE_DATA);

	return inside_surface;
}


int32_t cca_is_face_isthmus(struct xvimage* cca_image, uint32_t pixnumber, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	list *l;
	face_desc *f;
	uint32_t isthmus=1;

	if( (UCHARDATA(cca_image)[pixnumber] & face_type) ==0)
		return 0;

	l=cca_list_subfaces_dimension_direct_under(pixnumber, face_type, rs, ps);

	while(!list_isempty(l))
	{
		f=(face_desc*)list_pop_pointer(l);
		if(	isthmus==1 &&
			ISFREE(f->face_type, cca_is_face_free(cca_image, NULL, f->pixnumber, rs, ps, f->face_type)) )
		{
			isthmus=0;
		}
	}

	list_delete(l, NO_FREE_DATA);

	return isthmus;
}


uint32_t cca_edge_cube_cardinal_containers(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	int32_t result=0;

	switch(face_type)
	{
		case(CC_AX):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0)) result++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0)) result++;
			if(coord_y>0 && coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-rs-ps]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		case(CC_AY):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0)) result++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0)) result++;
			if(coord_x>0 && coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-1-ps]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		case(CC_AZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0)) result++;
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0)) result++;
			if(coord_x>0 && coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-1-rs]&CC_VOL)!=0)) result++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0) result++;
			break;

		default:
			fprintf(stderr, "cca_cardinal_containers() : Given face_type is incorrect or not yet implemented.\n");
			return(0);
	}
	return(result);
}

/*!	\brief Is the face free?

	\param cca_image The cubical complex containing the face to test.
	\param pixnumber The voxel containing the face to test.
	\param rs The size of a row of the image.
	\param ps The size of a plane of the image.
	\param face_type Which face to test (CC_FXY, CC_FXZ, CC_FYZ, CC_AX, CC_AY, CC_AZ, CC_PT). A volume is never free. A border face can be free.

	\ingroup complex
	\ingroup collapse

	\return The direction in which the face can be collapsed (see libcollapse.h for more details).
	\author John Chaussard
*/
unsigned char cca_is_face_free(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type)
{
	int32_t i, j, k, border;

	if( (UCHARDATA(cca_image)[pixnumber] & face_type) == 0)
		return CC_NO_SUCH_FACE;

	if( inhibit!=NULL && (UCHARDATA(inhibit)[pixnumber]&face_type)!=0)
		return CC_NOT_FREE_PROTECTED;

	k=pixnumber/ps;
	j=(pixnumber-k*ps)/rs;
	i=pixnumber-k*ps-j*rs;

	border=cca_is_face_onborder(cca_image, face_type, i, j, k);

	if(border!=0)
	{
		switch(face_type)
		{
			//Dont simplify these expressions as they correspond to specific codes used later...
			case (CC_FXY):
				if(border==Z_BORDER_MIN)
					return( ( (UCHARDATA(cca_image)[pixnumber]) & CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				else if(border==Z_BORDER_MAX)
					return( (UCHARDATA(cca_image)[pixnumber-ps]) & CC_VOL);
				else
					fprintf(stderr, "cca_is_face_free(): Unknown configuration given.\n");
					return(CC_NOT_FREE_NONEIGHBOUR);
				break;

			case (CC_FXZ):
				if(border==Y_BORDER_MIN)
					return( ( (UCHARDATA(cca_image)[pixnumber])& CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				else if(border==Y_BORDER_MAX)
					return( (UCHARDATA(cca_image)[pixnumber-rs]) & CC_VOL);
				else
					fprintf(stderr, "cca_is_face_free(): Unknown configuration given (2).\n");
					return(CC_NOT_FREE_NONEIGHBOUR);
				break;

			case (CC_FYZ):
				if(border==X_BORDER_MIN)
					return( ( (UCHARDATA(cca_image)[pixnumber]) & CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				else if(border==X_BORDER_MAX)
					return( (UCHARDATA(cca_image)[pixnumber-1]) & CC_VOL);
				else
					fprintf(stderr, "cca_is_face_free(): Unknown configuration given (3).\n");
					return(CC_NOT_FREE_NONEIGHBOUR);
				break;

			case (CC_AX):
				switch(border)
				{
					case (Y_BORDER_MIN | Z_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) ) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4));
						break;

					case (Y_BORDER_MIN | Z_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) ) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						break;

					case (Y_BORDER_MAX | Z_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) ) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) );
						break;

					case (Y_BORDER_MAX | Z_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) ) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						break;

					case (Y_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						break;

					case (Y_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						break;

					case (Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) );
						break;

					case (Z_BORDER_MAX):
						if ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
						break;

					default:
						fprintf(stderr, "cca_is_face_free(): Unknown configuration given (4).\n");
						return(CC_NOT_FREE_NONEIGHBOUR);
						break;
				}
				break;

			case (CC_AY):
				switch(border)
				{
					case (X_BORDER_MIN | Z_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						break;

					case (X_BORDER_MIN | Z_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MAX | Z_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						break;

					case (X_BORDER_MAX | Z_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) ^ ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						break;

					case (Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1 )
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						break;

					case (Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
						break;

					default:
						fprintf(stderr, "cca_is_face_free(): Unknown configuration given (5).\n");
						return(CC_NOT_FREE_NONEIGHBOUR);
						break;
				}
				break;

			case (CC_AZ):
				switch(border)
				{
					case (X_BORDER_MIN | Y_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
					break;

					case (X_BORDER_MIN | Y_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) ^ ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MIN):
						return( ( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) ^ ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MAX):
						return( ( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) ^ ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) ) | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1 )
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						break;

					case (X_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						break;

					case (Y_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) );
						break;

					case (Y_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
							return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
						break;

					default:
						fprintf(stderr, "cca_is_face_free(): Unknown configuration given (6).\n");
						return(CC_NOT_FREE_NONEIGHBOUR);
						break;
				}
				break;

			case (CC_PT):
				switch(border)
				{
					case (X_BORDER_MIN | Y_BORDER_MIN | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MIN | Y_BORDER_MIN | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MIN | Y_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MIN | Y_BORDER_MAX | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MIN | Y_BORDER_MAX | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MIN | Y_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MIN | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MIN | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MIN | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MIN | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MAX | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MAX | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX | Y_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (X_BORDER_MAX | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (X_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (Y_BORDER_MIN | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (Y_BORDER_MIN | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (Y_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (Y_BORDER_MAX | Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (Y_BORDER_MAX | Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (Y_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					case (Z_BORDER_MIN):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) );
						break;

					case (Z_BORDER_MAX):
						if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP )
							return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						else
							return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
						break;

					default:
						fprintf(stderr, "cca_is_face_free(): Unknown configuration given (7).\n");
						return(CC_NOT_FREE_NONEIGHBOUR);
						break;
				}
				break;

			default:
				fprintf(stderr, "cca_is_face_free(): Unknown configuration given (8).\n");
				return(CC_NOT_FREE_NONEIGHBOUR);
		}
	}

	else
	{
		switch(face_type)
		{
			case (CC_FXY):
				//We look the two neighbours (over and under) of the face, in order to see if it contains the volume... To do so, we look the actual voxel and its upper neighbour.
				//In order to have the face free, it needs to be contained in only one, and one, volume.
				return( ( ((UCHARDATA(cca_image)[pixnumber])^(UCHARDATA(cca_image)[pixnumber-ps])) & CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				break;

			case (CC_FXZ):
				return( ( ((UCHARDATA(cca_image)[pixnumber])^(UCHARDATA(cca_image)[pixnumber-rs])) & CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				break;

			case (CC_FYZ):
				return( ( ((UCHARDATA(cca_image)[pixnumber])^(UCHARDATA(cca_image)[pixnumber-1])) & CC_VOL) | ( ((UCHARDATA(cca_image)[pixnumber]) & CC_VOL) >> 1) );
				break;

			case (CC_AX):
				//We look at the four faces sharing this edge... If there is only one face present, it means the edge is free...
				if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 2) == CC_EDGE_COLL_DIR1)
					return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
				else
					return( ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 4) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FXZ) >> 3) );
				break;

			case (CC_AY):
				if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 1) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
					return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
				else
					return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXY) >> 4) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_FYZ) >> 4) );
				break;

			case (CC_AZ):
				if( ((UCHARDATA(cca_image)[pixnumber] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 3) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 3) == CC_EDGE_COLL_DIR1)
					return( CC_EDGE_COLL_DIR1 | ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
				else
					return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_FXZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber] & CC_FYZ) >> 5) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_FYZ) >> 4) );
				break;

			case (CC_PT):
				//Look at the six edges sharing this point...
				if( ((UCHARDATA(cca_image)[pixnumber] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 4) + ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) << 2) + ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY) << 3) + ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 4) == CC_VERT_COLL_XUP)
					return( CC_VERT_COLL_XUP | ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
				else
					return( ((UCHARDATA(cca_image)[pixnumber-1] & CC_AX) >> 3) | ((UCHARDATA(cca_image)[pixnumber] & CC_AY) >> 1) | ((UCHARDATA(cca_image)[pixnumber-rs] & CC_AY)) | ((UCHARDATA(cca_image)[pixnumber] & CC_AZ) << 2) | ((UCHARDATA(cca_image)[pixnumber-ps] & CC_AZ) << 3) );
				break;

			default:
				fprintf(stderr, "cca_is_face_free(): Unknown configuration given (8).\n");
				return(CC_NOT_FREE_NONEIGHBOUR);

		}
	}

	fprintf(stderr, "cca_is_face_free(): Unknown configuration given (9).\n");
	return(CC_NOT_FREE_NONEIGHBOUR);
}

int32_t cca_scan_all_vertex_into_complex(struct xvimage *cca_image, complexe *result)
{
	uint32_t rs, cs, d, ps, N, l;
	list *added_pt;

	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=rs*cs;
	N=ps*d;

	//The list with all vertices which were added temporarily to the image
	added_pt=list_newlist(MODE_FIFO, 100);
	if(added_pt==NULL)
	{
		fprintf(stderr, "cca_scan_all_vertex_into_complex(): Error in list creation.\n");
		return(-1);
	}

	//Scan the image and list its vertices... If a vertice should be but is not in the image, we temporarily add it to the image...
	for(l=0; l<N; l++)
	{
		if(UCHARDATA(cca_image)[l]>0)
		{
			if( (UCHARDATA(cca_image)[l]&CC_VOL) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+1)); UCHARDATA(cca_image)[l+1] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs)); UCHARDATA(cca_image)[l+rs] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps)); UCHARDATA(cca_image)[l+ps] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs+1)); UCHARDATA(cca_image)[l+rs+1] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps+1)); UCHARDATA(cca_image)[l+ps+1] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs+ps)); UCHARDATA(cca_image)[l+rs+ps] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs+ps+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs+ps+1)); UCHARDATA(cca_image)[l+rs+ps+1] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_FXY) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+1)); UCHARDATA(cca_image)[l+1] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs)); UCHARDATA(cca_image)[l+rs] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs+1)); UCHARDATA(cca_image)[l+rs+1] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_FXZ) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+1)); UCHARDATA(cca_image)[l+1] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps)); UCHARDATA(cca_image)[l+ps] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps+1)); UCHARDATA(cca_image)[l+ps+1] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_FYZ) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs)); UCHARDATA(cca_image)[l+rs] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps)); UCHARDATA(cca_image)[l+ps] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs+ps)); UCHARDATA(cca_image)[l+rs+ps] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_AX) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+1]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+1)); UCHARDATA(cca_image)[l+1] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_AY) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+rs]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+rs)); UCHARDATA(cca_image)[l+rs] |= CC_PT;}
			}

			if( (UCHARDATA(cca_image)[l]&CC_AZ) != 0)
			{
				if( (UCHARDATA(cca_image)[l]&CC_PT) == 0) {list_push_uint32_t(added_pt, l); UCHARDATA(cca_image)[l] |= CC_PT;}
				if( (UCHARDATA(cca_image)[l+ps]&CC_PT) == 0) {list_push_uint32_t(added_pt, (l+ps)); UCHARDATA(cca_image)[l+ps] |= CC_PT;}
			}

			if(complexe_add_element(result, l, CC_PT) <0)
			{
				fprintf(stderr, "cca_sca_vertex(): could not add vertice to complexe.\n");
				return(-1);
			}
		}
	}

	//Remove from image the temporarily added points
	while(!list_isempty(added_pt))
	{
		l=list_pop_uint32_t(added_pt);
		UCHARDATA(cca_image)[l] = UCHARDATA(cca_image)[l] - CC_PT;
	}

	//End
	list_delete(added_pt, NO_FREE_DATA);

	return(0);
}


//only for 1d complexes
//optimization of complexe simple curve should have been performed before
complexe* complexe_perform_curve_subsampling(complexe *cpl, uint32_t rs, uint32_t ps, double tolerance)
{
	uint32_t num_pt, start_tab_pt, i, j, pt, *R, cpt, x, y, z;
	int32_t nseg, *X, *Y, *Z, *temp;
	complexe *result;


	result = complexe_new_complexe();
	if(!result)
	{
		fprintf(stderr, "complexe_perform_curve_subsampling(): Memory allocation error.\n");
		return(NULL);
	}
	result->TYP_COMPLEXE = GENERAL_COMPLEXE;

	temp=(int32_t*)malloc(3*sizeof(int32_t)*cpl->num_edges_pts);
	if(!temp)
	{
		fprintf(stderr, "complexe_perform_curve_subsampling(): Memory allocation error.\n");
		return(NULL);
	}

	R=(uint32_t*)malloc(sizeof(int32_t)*cpl->num_edges_pts);
	if(!R)
	{
		fprintf(stderr, "complexe_perform_curve_subsampling(): Memory allocation error.\n");
		return(NULL);
	}

	X = temp;
	Y = X + cpl->num_edges_pts;
	Z = Y + cpl->num_edges_pts;

	for(i=0; i<cpl->num_edges; i++)
	{
		num_pt=complexe_get_numvertex_of_element(cpl, i, GC_EDGE);
		start_tab_pt=complexe_get_vertex_position_in_element(cpl, i, 0, GC_EDGE);

		assert(num_pt <= cpl->num_edges_pts); //just in case, for debug...

		//The curve, which is 6-connected, should be 26-connected...We need to remove some points
		pt = (cpl->tab_edges_pts[start_tab_pt]);
		X[0] = (int32_t)(pt % rs);
		Z[0] = (int32_t)(pt / ps);
		Y[0] = (int32_t)((pt % ps) / rs);

		pt = (cpl->tab_edges_pts[start_tab_pt+1]);
		X[1] = (int32_t)(pt % rs);
		Z[1] = (int32_t)(pt / ps);
		Y[1] = (int32_t)((pt % ps) / rs);

		cpt=2;

		for(j=2; j<num_pt; j++)
		{
			pt = (cpl->tab_edges_pts[start_tab_pt+j]);
			x=(int32_t)(pt % rs);
			z=(int32_t)(pt / ps);
			y=(int32_t)((pt % ps) / rs);
			if(abs(x-X[cpt-2])<=1 && abs(y-Y[cpt-2])<=1 && abs(z-Z[cpt-2])<=1) //The current point is 26-neighbor with the last last point, so replace last point by current point
			{
				cpt--;
			}
			X[cpt] = (int32_t)(pt % rs);
			Z[cpt] = (int32_t)(pt / ps);
			Y[cpt++] = (int32_t)((pt % ps) / rs);
		}


//		for(j=0; j<num_pt; j++)
//		{
//			pt = (cpl->tab_edges_pts[start_tab_pt+j]);
//			X[j] = (int32_t)(pt % rs);
//			Z[j] = (int32_t)(pt / ps);
//			Y[j] = (int32_t)((pt % ps) / rs);
//		}
//
//		cpt=num_pt;

		nseg = CoverByDSSs3D(cpt, X, Y, Z, tolerance);


		for(j=0; j<(uint32_t)nseg; j++)
		{
			R[j] = (uint32_t)(X[j] + Y[j]*rs + Z[j]*ps);
		}

		if(complexe_add_array_element(result, R, (uint32_t)nseg, GC_EDGE)!=0)
		{
			fprintf(stderr, "complexe_perform_curve_subsampling(): Error in subfunction.\n");
			return(NULL);
		}

		if(complexe_close_element(result, GC_EDGE)!=0)
		{
			fprintf(stderr, "complexe_perform_curve_subsampling(): Error in subfunction.\n");
			return(NULL);
		}


	}

	free(temp);
	free(R);

	return(result);
}



uint32_t complexe_optimize_simple_curve(complexe *cpl, uint32_t rs, uint32_t ps)
{
	int32_t *tab_vtx;
	uint32_t *tabpt, *tabresult;
	uint32_t numpt, i, j, pos1, pos2, decal, cpt;
	int32_t start, next;

	//Compute vertex array
	if(cpl->num_pt_obj==0)
		complexe_compute_vertex_array(cpl, rs, ps);


	//prepare some arrays
	tab_vtx = (int32_t*)calloc(3*(cpl->num_pt_obj), sizeof(int32_t));
	if(tab_vtx==NULL)
	{
		fprintf(stderr, "complexe_optimize_simple_curve(): memory allocation error.\n");
		return(1);
	}

	for(i=0; i<cpl->num_pt_obj; i++)
	{
		tab_vtx[3*i]=cpl->tab_pt_obj[i];
		tab_vtx[3*i+1]=-1;
		tab_vtx[3*i+2]=-1;
	}



	//For each point of the curve, find its successor, and add the pair to the array
	for(j=0; j<3; j++)
	{
		switch(j)
		{
			case(0):
				tabpt=cpl->tab_ax;
				numpt=cpl->num_ax;
				decal=1;
				break;

			case(1):
				tabpt=cpl->tab_ay;
				numpt=cpl->num_ay;
				decal=rs;
				break;

			case(2):
				tabpt=cpl->tab_az;
				numpt=cpl->num_az;
				decal=ps;
				break;

			default:
				free(tab_vtx);
				fprintf(stderr, "complexe_optimize_simple_curve(): error on value for variable in switch.\n");
				return(1);
		}

		for(i=0; i<numpt; i++)
		{
			pos1=dichotomic_research_on_uint32t(cpl->tab_pt_obj, tabpt[i], cpl->num_pt_obj);
			if(pos1==0)
			{
				free(tab_vtx);
				fprintf(stderr, "complexe_optimize_simple_curve(): dichotomic research failed.\n");
				return(1);
			}
			pos1=pos1-1;

			pos2=dichotomic_research_on_uint32t(cpl->tab_pt_obj, tabpt[i]+decal, cpl->num_pt_obj);
			if(pos2==0)
			{
				free(tab_vtx);
				fprintf(stderr, "complexe_optimize_simple_curve(): dichotomic research failed.\n");
				return(1);
			}
			pos2=pos2-1;

			if(tab_vtx[3*pos1+1]==-1)
			{
				tab_vtx[3*pos1+1]=pos2;
			}
			else if(tab_vtx[3*pos1+2]==-1)
			{
				tab_vtx[3*pos1+2]=pos2;
			}
			else
			{
				free(tab_vtx);
				fprintf(stderr, "complexe_optimize_simple_curve(): failed because curve is not simple (vertex with three edges on it).\n");
				return(1);
			}

			if(tab_vtx[3*pos2+1]==-1)
			{
				tab_vtx[3*pos2+1]=pos1;
			}
			else if(tab_vtx[3*pos2+2]==-1)
			{
				tab_vtx[3*pos2+2]=pos1;
			}
			else
			{
				free(tab_vtx);
				fprintf(stderr, "complexe_optimize_simple_curve(): failed because curve is not simple (vertex with three edges on it) (2).\n");
				return(1);
			}
		}
	}


	//The final curve array
	tabresult=(uint32_t*)calloc(cpl->num_ax + cpl->num_ay + cpl->num_az + 1, sizeof(uint32_t));
	if(tabresult==NULL)
	{
		free(tab_vtx);
		fprintf(stderr, "complexe_optimize_simple_curve(): memory allocation error(2).\n");
		return(1);
	}


	//Look for the start of the curve
	start=0;
	for(i=1; i<cpl->num_pt_obj; i++)
	{
		if(tab_vtx[3*i+2] ==-1)
			start=i;
	}

	next=tab_vtx[3*start+1];

	tab_vtx[3*start+1]=-1;
	tab_vtx[3*start+2]=-1;


	//Either we found a start, or if we didn't, it means we have a cycle... no problem, let's start at 0
	tabresult[0]=cpl->tab_pt_obj[start];
	cpt=1;
	while(next!=-1)
	{
		if(tab_vtx[3*next+1] == start) tab_vtx[3*next+1]=-1;
		else if(tab_vtx[3*next+2] == start) tab_vtx[3*next+2]=-1;
		else if(tab_vtx[3*next+1]!=-1 && tab_vtx[3*next+2]!=-1)
		{
			free(tab_vtx);
			free(tabresult);
			fprintf(stderr, "complexe_optimize_simple_curve(): data array contains errors.\n");
			return(1);
		}

		if(cpt >= (cpl->num_ax + cpl->num_ay + cpl->num_az + 1))
		{
			free(tab_vtx);
			free(tabresult);
			fprintf(stderr, "complexe_optimize_simple_curve(): result array does not contain enough free cells, or data array contains errors.\n");
			return(1);
		}

		tabresult[cpt++]=cpl->tab_pt_obj[next];

		start=next;

		if(tab_vtx[3*start+1] !=-1) next=tab_vtx[3*start+1];
		else next=tab_vtx[3*start+2];

		tab_vtx[3*start+1]=-1;
		tab_vtx[3*start+2]=-1;
	}


	free(tab_vtx);

	if(cpt!=(cpl->num_ax + cpl->num_ay + cpl->num_az + 1))
	{
		free(tabresult);
		fprintf(stderr, "complexe_optimize_simple_curve(): data array contains errors (2).\n");
		return(1);
	}


	cpl->num_ax=0;
	cpl->num_ay=0;
	cpl->num_az=0;
	if(cpl->tab_ax!=NULL) free(cpl->tab_ax);
	if(cpl->tab_ay!=NULL) free(cpl->tab_ay);
	if(cpl->tab_az!=NULL) free(cpl->tab_az);

	cpl->TYP_COMPLEXE=GENERAL_COMPLEXE;

	if(complexe_add_array_element(cpl, tabresult, cpt, GC_EDGE)!=0)
	{
		free(tabresult);
		fprintf(stderr, "complexe_optimize_simple_curve(): error in subfunction.\n");
		return(1);
	}
	if(complexe_close_element(cpl, GC_EDGE)!=0)
	{
		free(tabresult);
		fprintf(stderr, "complexe_optimize_simple_curve(): error in subfunction.\n");
		return(1);
	}

	return(0);
}



int32_t cca_get_all_edges_with_given_cardinalcontainer(struct xvimage *cca_image, complexe* result, uint32_t cardinal_container)
{
	uint32_t rs, ps, i, j, k, cs, px, d;

	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=rs*cs;

	px=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if((UCHARDATA(cca_image)[px]&CC_AX)!=0)
					if(cca_cardinal_containers(cca_image, px, i, j, k, CC_AX, rs, ps)==cardinal_container)
						if(complexe_add_element(result, px, CC_AX)!=0)
						{
							fprintf(stderr, "cca_get_all_surfaces_intersections(): add element failed (1).\n");
							return(-1);
						}

				if((UCHARDATA(cca_image)[px]&CC_AY)!=0)
					if (cca_cardinal_containers(cca_image, px, i, j, k, CC_AY, rs, ps)==cardinal_container)
						if(complexe_add_element(result, px, CC_AY)!=0)
						{
							fprintf(stderr, "cca_get_all_surfaces_intersections(): add element failed (2).\n");
							return(-1);
						}

				if((UCHARDATA(cca_image)[px]&CC_AZ)!=0)
					if (cca_cardinal_containers(cca_image, px, i, j, k, CC_AZ, rs, ps)==cardinal_container)
						if(complexe_add_element(result, px, CC_AZ)!=0)
						{
							fprintf(stderr, "cca_get_all_surfaces_intersections(): add element failed (3).\n");
							return(-1);
						}
				px++;
			}

	return(0);
}


list* cca_list_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	face_desc* data_result;
	list *containers;

	containers=list_newlist(MODE_FIFO, 6);
	switch(face_type)
	{
		case(CC_PT):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_AX)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_1).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-1;
				data_result->face_type=CC_AX;
				list_push_pointer(containers, data_result);
			}
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_AY)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_2).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-rs;
				data_result->face_type=CC_AY;
				list_push_pointer(containers, data_result);
			}
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_AZ)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_3).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-ps;
				data_result->face_type=CC_AZ;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_AX)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_4).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_AX;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_AY)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_5).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_AY;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_AZ)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (CC_PT_6).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_AZ;
				list_push_pointer(containers, data_result);
			}
			break;

		case(CC_AX):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FXY)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (1).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-rs;
				data_result->face_type=CC_FXY;
				list_push_pointer(containers, data_result);
			}
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FXZ)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (2).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-ps;
				data_result->face_type=CC_FXZ;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (3).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FXY;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (4).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FXZ;
				list_push_pointer(containers, data_result);
			}
			break;

		case(CC_AY):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXY)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (5).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-1;
				data_result->face_type=CC_FXY;
				list_push_pointer(containers, data_result);
			}
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FYZ)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (6).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-ps;
				data_result->face_type=CC_FYZ;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (7).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FXY;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (8).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FYZ;
				list_push_pointer(containers, data_result);
			}
			break;

		case(CC_AZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXZ)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (9).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-1;
				data_result->face_type=CC_FXZ;
				list_push_pointer(containers, data_result);
			}
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FYZ)!=0))
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (10).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber-rs;
				data_result->face_type=CC_FYZ;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (11).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FXZ;
				list_push_pointer(containers, data_result);
			}
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0)
			{
				data_result=(face_desc*)calloc(1, sizeof(face_desc));
				if(data_result==NULL)
				{
					fprintf(stderr, "cca_list_container(): Memory allocation error (12).\n");
					return(NULL);
				}
				data_result->pixnumber=pixnumber;
				data_result->face_type=CC_FYZ;
				list_push_pointer(containers, data_result);
			}
			break;

		default:
			fprintf(stderr, "cca_list_containers() : Given face_type is incorrect or not yet implemented.\n");
			return(NULL);
	}
	return(containers);
}


uint32_t cca_count_face_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	uint32_t r=0;
	switch(face_type)
	{
		case(CC_VOL):
			break;

		case(CC_FXY):
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			break;

		case(CC_FXZ):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			break;

		case(CC_FYZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			break;

		case(CC_AX):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FXY)!=0))
				r++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FXZ)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0)
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0)
				r++;
			break;

		case(CC_AY):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXY)!=0))
				r++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_FYZ)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXY)!=0)
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0)
				r++;
			break;

		case(CC_AZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_FXZ)!=0))
				r++;
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_FYZ)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FXZ)!=0)
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_FYZ)!=0)
				r++;
			break;

		default:
			fprintf(stderr, "cca_count_face_container() : Given face_type is incorrect or not yet implemented.\n");
			return(-1);
	}
	return(r);
}


uint32_t cca_count_cube_container(struct xvimage* cca_image, uint32_t pixnumber, uint32_t coord_x, uint32_t coord_y, uint32_t coord_z, unsigned char face_type, uint32_t rs, uint32_t ps)
{
	uint32_t r=0;
	switch(face_type)
	{
		case(CC_AX):
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0))
				r++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			if(coord_y>0 && coord_z>0 && (UCHARDATA(cca_image)[pixnumber-rs-ps]&CC_VOL)!=0)
				r++;
			break;

		case(CC_AY):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0))
				r++;
			if(coord_z>0 && ((UCHARDATA(cca_image)[pixnumber-ps]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			if(coord_x>0 && coord_z>0 && (UCHARDATA(cca_image)[pixnumber-1-ps]&CC_VOL)!=0)
				r++;
			break;

		case(CC_AZ):
			if(coord_x>0 && ((UCHARDATA(cca_image)[pixnumber-1]&CC_VOL)!=0))
				r++;
			if(coord_y>0 && ((UCHARDATA(cca_image)[pixnumber-rs]&CC_VOL)!=0))
				r++;
			if((UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0)
				r++;
			if(coord_x>0 && coord_y>0 && (UCHARDATA(cca_image)[pixnumber-1-rs]&CC_VOL)!=0)
				r++;
			break;

		default:
			fprintf(stderr, "cca_cardinal_containers() : Given face_type is incorrect or not yet implemented.\n");
			return(-1);
	}
	return(r);
}


int32_t cca_get_border_faces_parallel_to_face(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps, uint32_t *r1, uint32_t *r2)
{
	uint32_t numpix_x, numpix_y, numpix_z, deb, fin, milieu, tx, ty, tz, tpx, tpy, tpz;
	numpix_z=numpix/ps;
	numpix_y=(numpix%ps)/rs;
	numpix_x=numpix%rs;
	deb=0;

	switch(face_type)
	{
		case(CC_FXY):
			//Dichotomic research on border_face in order to find the greater XY face on same column
			//than numpix and just before it (same x, y and z just over).
			fin=border_faces->num_fxy;
			milieu=((fin-deb)/2)-(((fin-deb)/2)%2) +deb;
			tz=(border_faces->tab_fxy[milieu])/ps;
			ty=((border_faces->tab_fxy[milieu])%ps)/rs;
			tx=(border_faces->tab_fxy[milieu])%rs;
			tpz=(border_faces->tab_fxy[milieu+1])/ps;
			while( ((tx!=numpix_x) || (ty!=numpix_y) || (tz>numpix_z) || (tpz<numpix_z)) && milieu!=deb )
			{
				if(tx!=numpix_x)
				{
					if(tx>numpix_x)
						fin=milieu;
					else
						deb=milieu;
				}
				else if(ty!=numpix_y)
				{
					if(ty>numpix_y)
						fin=milieu;
					else
						deb=milieu;
				}
				else
				{
					if(tz>numpix_z)
						fin=milieu;
					else
						deb=milieu;
				}

				milieu=((fin-deb)/2)-(((fin-deb)/2)%2) +deb;
				tz=(border_faces->tab_fxy[milieu])/ps;
				ty=((border_faces->tab_fxy[milieu])%ps)/rs;
				tx=(border_faces->tab_fxy[milieu])%rs;
				tpz=(border_faces->tab_fxy[milieu+1])/ps;
			}

			if( ((tx!=numpix_x) || (ty!=numpix_y) || (tz>numpix_z) || (tpz<numpix_z)) && milieu==deb )
			{
				fprintf(stderr, "cca_get_border_faces_parallel_to_face(): Could not find requested value.\n");
				return(-1);
			}

			*r1=tz;
			*r2=tpz;

			break;

		case(CC_FYZ):
			fin=border_faces->num_fyz;
			milieu=((fin-deb)/2)-(((fin-deb)/2)%2) +deb;
			tz=(border_faces->tab_fyz[milieu])/ps;
			ty=((border_faces->tab_fyz[milieu])%ps)/rs;
			tx=(border_faces->tab_fyz[milieu])%rs;
			tpx=(border_faces->tab_fyz[milieu+1])%rs;
			while( ((ty!=numpix_y) || (tz!=numpix_z) || (tx>numpix_x) || (tpx<numpix_x)) && milieu!=deb )
			{
				if(ty!=numpix_y)
				{
					if(ty>numpix_y)
						fin=milieu;
					else
						deb=milieu;
				}
				else if(tz!=numpix_z)
				{
					if(tz>numpix_z)
						fin=milieu;
					else
						deb=milieu;
				}
				else
				{
					if(tx>numpix_x)
						fin=milieu;
					else
						deb=milieu;
				}

				milieu=((fin-deb)/2)-(((fin-deb)/2)%2) +deb;
				tz=(border_faces->tab_fyz[milieu])/ps;
				ty=((border_faces->tab_fyz[milieu])%ps)/rs;
				tx=(border_faces->tab_fyz[milieu])%rs;
				tpx=(border_faces->tab_fyz[milieu+1])%rs;
			}

			if( ((ty!=numpix_y) || (tz!=numpix_z) || (tx>numpix_x) || (tpx<numpix_x)) && milieu==deb )
			{
				fprintf(stderr, "cca_get_border_faces_parallel_to_face(): Could not find requested value (2).\n");
				return(-1);
			}

			*r1=tx;
			*r2=tpx;

			break;

		case(CC_FXZ):
			fin=border_faces->num_fxz;
			milieu=((fin-deb)/2)-(((fin-deb)/2)%2)+deb;
			tz=(border_faces->tab_fxz[milieu])/ps;
			ty=((border_faces->tab_fxz[milieu])%ps)/rs;
			tx=(border_faces->tab_fxz[milieu])%rs;
			tpy=((border_faces->tab_fxz[milieu+1])%ps)/rs;
			while( ((tz!=numpix_z) || (tx!=numpix_x) || (ty>numpix_y) || (tpy<numpix_y)) && milieu!=deb )
			{
				if(tz!=numpix_z)
				{
					if(tz>numpix_z)
						fin=milieu;
					else
						deb=milieu;
				}
				else if(tx!=numpix_x)
				{
					if(tx>numpix_x)
						fin=milieu;
					else
						deb=milieu;
				}
				else
				{
					if(ty>numpix_y)
						fin=milieu;
					else
						deb=milieu;
				}

				milieu=((fin-deb)/2)-(((fin-deb)/2)%2) +deb;
				tz=(border_faces->tab_fxz[milieu])/ps;
				ty=((border_faces->tab_fxz[milieu])%ps)/rs;
				tx=(border_faces->tab_fxz[milieu])%rs;
				tpy=((border_faces->tab_fxz[milieu+1])%ps)/rs;
			}

			if( ((tz!=numpix_z) || (tx!=numpix_x) || (ty>numpix_y) || (tpy<numpix_y)) && milieu==deb )
			{
				fprintf(stderr, "cca_get_border_faces_parallel_to_face(): Could not find requested value (3).\n");
				return(-1);
			}

			*r1=ty;
			*r2=tpy;

			break;

		default:
			fprintf(stderr, "cca_get_border_faces_parallel_to_face(): Unknown face type given.\n");
			return(0);
	}
	return(0);
}

//Attention, renvoit le depth+1 !!!
uint32_t cca_get_face_normaldepth(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps)
{
	uint32_t r1, r2, numpix_x, numpix_y, numpix_z;

	if(cca_get_border_faces_parallel_to_face(face_type, border_faces, numpix, rs, ps, &r1, &r2) <0)
	{
		fprintf(stderr, "cca_get_face_normaldepth(): cca_get_border_faces_parallel_to_face failed.\n");
		return(0);
	}


	switch(face_type)
	{
		case (CC_FXY):
			numpix_z=numpix/ps;
			if( numpix_z-r1 > r2-numpix_z )
				return(r2-numpix_z +1);
			else
				return(numpix_z-r1 +1);
		break;

		case (CC_FYZ):
			numpix_x=numpix%rs;
			if( numpix_x-r1 > r2-numpix_x )
				return(r2-numpix_x +1);
			else
				return(numpix_x-r1 +1);
		break;

		case(CC_FXZ):
			numpix_y=(numpix%ps)/rs;
			if( numpix_y-r1 > r2-numpix_y )
				return(r2-numpix_y +1);
			else
				return(numpix_y-r1 +1);
		break;

		default:
			fprintf(stderr, "cca_get_face_normaldepth(): Unknown face type given.\n");
			return(0);
	}
	return(0);
}


uint32_t cca_get_face_rvsa_depth(unsigned char face_type, complexe* border_faces, uint32_t numpix, uint32_t rs, uint32_t ps)
{
	uint32_t r1, r2;

	if(cca_get_border_faces_parallel_to_face(face_type, border_faces, numpix, rs, ps, &r1, &r2) <0)
	{
		fprintf(stderr, "cca_get_face_rvsa_depth(): cca_get_border_faces_parallel_to_face failed.\n");
		return(0);
	}


	return((r2-r1)/2 +1);
}


//XY faces are sorted by X, Y, Z
//YZ faces are sorted by Y, Z, X
//XZ faces are sorted by Z, X, Y
complexe* cca_compute_border_faces(struct xvimage* cca_image)
{
	unsigned char face_type;
	uint32_t i,j,k,limi,limj,limk,rs,ps,ci,cj,ck,pix,v,c;
	int32_t deb;
	complexe* result;

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cca_compute_border_faces(): Error when allocating new complex.\n");
		return(NULL);
	}

	rs=rowsize(cca_image);
	ps=rs*colsize(cca_image);
	ci=1;
	cj=rs;
	ck=ps;

	//First, we deal with the XY faces...
	face_type=CC_FXY;
	limk=depth(cca_image);
	limi=rowsize(cca_image);
	limj=colsize(cca_image);
	//Scan all faces, in column order...
	for(c=0;c<3;c++)
	{
		for(i=0; i<limi; i++)
			for(j=0; j<limj; j++)
			{
				k=0;
				deb=-1;
				pix=i*ci+j*cj+k*ck;
				while(k<limk)
				{
					//If a new set of non empty voxels is found, a new border face is found
					if( (UCHARDATA(cca_image)[pix]&CC_VOL)!=0 && deb==-1)
					{
						deb=1;
						if(complexe_add_element(result, pix, face_type)<0)
						{
							fprintf(stderr, "cca_compute_border_faces(): Error when adding face to complex.\n");
							return(NULL);
						}
					}
					else if((UCHARDATA(cca_image)[pix]&CC_VOL)==0 && deb!=-1)
					{
						if(complexe_add_element(result, pix, face_type)<0)
						{
							fprintf(stderr, "cca_compute_border_faces(): Error when adding face to complex.\n");
							return(NULL);
						}
						deb=-1;
					}
					k++;
					pix+=ck;
				}
			}
		//Then, swap the variables in order to process the other face types
		v=i;i=j;j=k;k=v;
		v=limi;limi=limj;limj=limk;limk=v;
		v=ci;ci=cj;cj=ck;ck=v;
		if(c==0)
			face_type=CC_FYZ;
		else
			face_type=CC_FXZ;
	}

	return result;
}


complexe* cca_compute_vise_axis_internal(struct xvimage* cca_image, complexe* border_faces)
{
	uint32_t i, rs, ps;
	complexe* result;

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cca_compute_vise_axis(): Error when allocating new complex.\n");
		return(NULL);
	}

	rs=rowsize(cca_image);
	ps=rs*colsize(cca_image);

	for(i=0; i<border_faces->num_fxy; i=i+2)
	{
		if( complexe_add_element(result, (((border_faces->tab_fxy[i+1] - border_faces->tab_fxy[i])/ps)/2)*ps+border_faces->tab_fxy[i], CC_FXY) <0)
		{
			fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed.\n");
			return(NULL);
		}

		if( ((border_faces->tab_fxy[i+1] - border_faces->tab_fxy[i])/ps)%2 != 0)
		{
			if( complexe_add_element(result, (((border_faces->tab_fxy[i+1] - border_faces->tab_fxy[i])/ps +1)/2)*ps+border_faces->tab_fxy[i], CC_FXY) <0)
			{
				fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed (2).\n");
				return(NULL);
			}
		}
	}


	for(i=0; i<border_faces->num_fyz; i=i+2)
	{
		if( complexe_add_element(result, ((border_faces->tab_fyz[i+1] - border_faces->tab_fyz[i])/2)+border_faces->tab_fyz[i], CC_FYZ) <0)
		{
			fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed (3).\n");
			return(NULL);
		}

		if( (border_faces->tab_fyz[i+1] - border_faces->tab_fyz[i])%2 != 0)
		{
			if( complexe_add_element(result, ((border_faces->tab_fyz[i+1] - border_faces->tab_fyz[i] +1)/2)+border_faces->tab_fyz[i], CC_FYZ) <0)
			{
				fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed (4).\n");
				return(NULL);
			}
		}
	}


	for(i=0; i<border_faces->num_fxz; i=i+2)
	{
		if( complexe_add_element(result, (((border_faces->tab_fxz[i+1] - border_faces->tab_fxz[i])/rs)/2)*rs+border_faces->tab_fxz[i], CC_FXZ) <0)
		{
			fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed (5).\n");
			return(NULL);
		}

		if( ((border_faces->tab_fxz[i+1] - border_faces->tab_fxz[i])/rs)%2 != 0)
		{
			if( complexe_add_element(result, (((border_faces->tab_fxz[i+1] - border_faces->tab_fxz[i])/rs +1)/2)*rs+border_faces->tab_fxz[i], CC_FXZ) <0)
			{
				fprintf(stderr, "cca_compute_vise_axis(): complexe_add_element() failed (6).\n");
				return(NULL);
			}
		}
	}

	return result;
}


complexe* cca_compute_vise_axis(struct xvimage* cca_image)
{
	complexe* result;
	complexe* border_faces=cca_compute_border_faces(cca_image);
	if(border_faces==NULL)
	{
		fprintf(stderr, "cca_compute_vise_axis(): cca_compute_border_faces failed.\n");
		return(NULL);
	}

	result=cca_compute_vise_axis_internal(cca_image, border_faces);
	complexe_free_complexe(border_faces);
	return(result);
}


complexe* cca_compute_vise_surfacic_axis(struct xvimage* cca_image, uint32_t alpha)
{
	complexe* result;
	complexe* border_faces=cca_compute_border_faces(cca_image);
	if(border_faces==NULL)
	{
		fprintf(stderr, "cca_compute_vise_surfacic_axis: cca_compute_border_faces failed.\n");
		return(NULL);
	}

	result=cca_compute_vise_surfacic_axis_internal(cca_image, alpha, border_faces);
	complexe_free_complexe(border_faces);
	return(result);
}

int32_t qsort_function_on_face_deep(const void* a, const void* b)
{
	if( (((face_deep*)(a))->deepness) < ( ((face_deep*)(b))->deepness) )
		return (-1);
	else if( (((face_deep*)(a))->deepness) > ( ((face_deep*)(b))->deepness) )
		return(1);
	else
		return(0);
}

complexe* cca_compute_vise_surfacic_axis_internal(struct xvimage* cca_image, uint32_t alpha, complexe *border_faces)
{
	complexe *va, *result;
	uint32_t val, v, i, ps, rs, j, numxy, numxz, numyz, cxy, cyz, cxz, vmax, vprev, N;
	face_deep *couplexy, *couplexz, *coupleyz;
	struct xvimage* debug;

	va=cca_compute_vise_axis_internal(cca_image, border_faces);
	if(va==NULL)
	{
		fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_compute_vise_axis_internal failed.\n");
		return(NULL);
	}


	couplexy = (face_deep*)calloc(va->num_fxy, sizeof(face_deep));
	couplexz = (face_deep*)calloc(va->num_fxz, sizeof(face_deep));
	coupleyz = (face_deep*)calloc(va->num_fyz, sizeof(face_deep));
	numxy=0;
	numxz=0;
	numyz=0;


	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_new_complexe failed.\n");
		return(NULL);
	}

	rs=rowsize(cca_image);
	ps=rs*colsize(cca_image);

	for(i=0; i<va->num_fxy; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			val=0;
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fxy[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxy[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed.\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (2).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i] +rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (3).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (4).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] +1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (5).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fxy[i] >= ps) && ((UCHARDATA(cca_image)[va->tab_fxy[i] -ps] & CC_VOL) !=0))
			{
				val=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxy[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (6).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i] -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (7).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i] +rs -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (8).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (9).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] +1 -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (10).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			//We add it to the array.... more filtering coming...
			couplexy[numxy].pixnum=va->tab_fxy[i];
			couplexy[numxy++].deepness=val;
			/*if( complexe_add_element(result, va->tab_fxy[i], CC_FXY) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed.\n");
				return(NULL);
			}*/
		}
	}

	couplexy=(face_deep*)realloc(couplexy, sizeof(face_deep)*numxy);
	if(couplexy==NULL)
	{
		fprintf(stderr, "cca_compute_vise_surfacic_axis_internal(): realloc() failed.\n");
		return(NULL);
	}

	for(i=0; i<va->num_fxz; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			val=0;
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fxz[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (11).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (12).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] +ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (13).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (14).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] +1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (15).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fxz[i] >= rs) && ((UCHARDATA(cca_image)[va->tab_fxz[i] -rs] & CC_VOL) !=0))
			{
				val=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (16).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (17).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] +ps -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (18).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (19).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] +1 -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (20).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			couplexz[numxz].pixnum=va->tab_fxz[i];
			couplexz[numxz++].deepness=val;
			/*if( complexe_add_element(result, va->tab_fxz[i], CC_FXZ) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed (2).\n");
				return(NULL);
			}*/
		}
	}

	couplexz=(face_deep*)realloc(couplexz, sizeof(face_deep)*numxz);

	for(i=0; i<va->num_fyz; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			val=0;
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fyz[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fyz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (21).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (22).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i] +ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (23).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (24).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i] +rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (25).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fyz[i] >= 1) && ((UCHARDATA(cca_image)[va->tab_fyz[i] -1] & CC_VOL) !=0))
			{
				val=cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fyz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (26).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i] -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (27).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i] +ps -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (28).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i] -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (29).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;

				v=cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i] +rs -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (30).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<val)
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			coupleyz[numyz].pixnum=va->tab_fyz[i];
			coupleyz[numyz++].deepness=val;
			/*if( complexe_add_element(result, va->tab_fyz[i], CC_FYZ) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed (3).\n");
				return(NULL);
			}*/
		}
	}

	coupleyz=(face_deep*)realloc(coupleyz, sizeof(face_deep)*numyz);

	//We make another filtering....
	//We are going to check wether the faces we kept really denote a surface or a corner...

	complexe_free_complexe(va);

	qsort(couplexy, numxy, sizeof(face_deep), qsort_function_on_face_deep);
	qsort(coupleyz, numyz, sizeof(face_deep), qsort_function_on_face_deep);
	qsort(couplexz, numxz, sizeof(face_deep), qsort_function_on_face_deep);

	vmax=0;
	if(numxy>0)
		vmax=couplexy[numxy-1].deepness;
	if(numxz>0 && couplexz[numxz-1].deepness > vmax)
		vmax=couplexz[numxz-1].deepness;
	if(numyz>0 && coupleyz[numyz-1].deepness > vmax)
		vmax=coupleyz[numyz-1].deepness;

	cxy=0;cyz=0;cxz=0;
	v=0;

	debug=allocimage(NULL, rowsize(cca_image), colsize(cca_image), depth(cca_image), VFF_TYP_1_BYTE);
	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);


	//Now, we are going to filter the remaining faces
	cca_collapse_method_facesandsubfaces2(cca_image, debug, 1);
	//writeimage(cca_image, "debug.cc");

	while(v<vmax)
	{
		vprev=v;
		v=vmax;
		//Choose the lowest value for deepness
		if(cxy<numxy) v=couplexy[cxy].deepness;
		if(cxz<numxz && couplexz[cxz].deepness<=v) v=couplexz[cxz].deepness;
		if(cyz<numyz && coupleyz[cyz].deepness<=v) v=coupleyz[cyz].deepness;

		fprintf(stdout, "Turn, v=%d (max:%d)\n", v, vmax);

		//And skeletonise the object (collapse) by v-vprev(+1)
		if(v-vprev!=0)
			cca_collapse_method_facesandsubfaces(cca_image, debug, v-vprev);

		while(cxy<numxy && couplexy[cxy].deepness == v)
		{
			if( (UCHARDATA(cca_image)[couplexy[cxy].pixnum] & CC_FXY) != 0)
			{
				complexe_add_element(result, couplexy[cxy].pixnum, CC_FXY);
				UCHARDATA(debug)[couplexy[cxy].pixnum]|=CC_FXY;
				//fprintf(stdout, "%d\n", v);
			}
			cxy++;
		}

		while(cxz<numxz && couplexz[cxz].deepness == v)
		{
			if( (UCHARDATA(cca_image)[couplexz[cxz].pixnum] & CC_FXZ) != 0)
			{
				complexe_add_element(result, couplexz[cxz].pixnum, CC_FXZ);
				UCHARDATA(debug)[couplexy[cxy].pixnum]|=CC_FXZ;
				//fprintf(stdout, "%d\n", v);
			}
			cxz++;
		}

		while(cyz<numyz && coupleyz[cyz].deepness == v)
		{
			if( (UCHARDATA(cca_image)[coupleyz[cyz].pixnum] & CC_FYZ) != 0)
			{
				complexe_add_element(result, coupleyz[cyz].pixnum, CC_FYZ);
				UCHARDATA(debug)[couplexy[cxy].pixnum]|=CC_FYZ;
				//fprintf(stdout, "%d\n", v);
			}
			cyz++;
		}

		for(i=0; i<N; i++)
		{
			if( (UCHARDATA(cca_image)[i]&CC_FXY)!=0)
			{
				if(cca_get_face_normaldepth(CC_FXY, border_faces, i, rs, ps) < v)
					UCHARDATA(debug)[i]|=CC_FXY;
			}

			if( (UCHARDATA(cca_image)[i]&CC_FXZ)!=0)
			{
				if(cca_get_face_normaldepth(CC_FXZ, border_faces, i, rs, ps) < v)
					UCHARDATA(debug)[i]|=CC_FXZ;
			}

			if( (UCHARDATA(cca_image)[i]&CC_FYZ)!=0)
			{
				if(cca_get_face_normaldepth(CC_FYZ, border_faces, i, rs, ps) < v)
					UCHARDATA(debug)[i]|=CC_FYZ;
			}
		}

	}


	free(couplexy);
	free(couplexz);
	free(coupleyz);
	return result;
}



complexe* cca_compute_vise_surfacic_axis_internal2(struct xvimage* cca_image, uint32_t alpha, complexe *border_faces)
{
	complexe *va, *result;
	uint32_t val, v, i, ps, rs, j;

	va=cca_compute_vise_axis_internal(cca_image, border_faces);
	if(va==NULL)
	{
		fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_compute_vise_axis_internal failed.\n");
		return(NULL);
	}

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_new_complexe failed.\n");
		return(NULL);
	}

	rs=rowsize(cca_image);
	ps=rs*colsize(cca_image);

	for(i=0; i<va->num_fxy; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			if(va->tab_fxy[i]==374531)
			{
				fprintf(stdout, "Bouh.\n");
			}
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fxy[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxy[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed.\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxy[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (2).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxy[i] +rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (3).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i]+rs, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxy[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (4).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxy[i] +1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (5).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] +1, rs, ps)!=(v-1))
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fxy[i] >= ps) && ((UCHARDATA(cca_image)[va->tab_fxy[i] -ps] & CC_VOL) !=0))
			{
				val=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxy[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (6).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxy[i] -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (7).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i] -ps, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxy[i] +rs -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (8).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fxy[i] +rs -ps, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxy[i] -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (9).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] -ps, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxy[i] +1 -ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (10).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxy[i] +1 -ps, rs, ps)!=(v-1))
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			if( complexe_add_element(result, va->tab_fxy[i], CC_FXY) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed.\n");
				return(NULL);
			}
		}
	}

	for(i=0; i<va->num_fxz; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fxz[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (11).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (12).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxz[i] +ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (13).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] +ps, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (14).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxz[i] +1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (15).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] +1, rs, ps)!=(v-1))
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fxz[i] >= rs) && ((UCHARDATA(cca_image)[va->tab_fxz[i] -rs] & CC_VOL) !=0))
			{
				val=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fxz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (16).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxz[i] -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (17).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] -rs, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fxz[i] +ps -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (18).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fxz[i] +ps -rs, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxz[i] -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (19).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] -rs, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fxz[i] +1 -rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (20).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FYZ, border_faces, va->tab_fxz[i] +1 -rs, rs, ps)!=(v-1))
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			if( complexe_add_element(result, va->tab_fxz[i], CC_FXZ) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed (2).\n");
				return(NULL);
			}
		}
	}

	for(i=0; i<va->num_fyz; i++)
	{
		//Look at the eight neighbour faces of the current face in order to check if it has the highest value...
		//Just to be able to break in a long processing...
		for(j=0; j<1; j++)
		{
			//If the current face is contained in first cube
			if( (UCHARDATA(cca_image)[va->tab_fyz[i]] & CC_VOL) !=0)
			{
				val=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fyz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (21).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fyz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (22).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fyz[i] +ps, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (23).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i] +ps, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fyz[i], rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (24).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i], rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fyz[i] +rs, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (25).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i] +rs, rs, ps)!=(v-1))
					break;
			}

			//If the current face is contained in second cube
			if( (va->tab_fyz[i] >= 1) && ((UCHARDATA(cca_image)[va->tab_fyz[i] -1] & CC_VOL) !=0))
			{
				val=cca_get_face_normaldepth(CC_FYZ, border_faces, va->tab_fyz[i], rs, ps);
				if(val==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (26).\n");
					return(NULL);
				}
				val--;
				//Take a look at the four faces of this cube which are neighbour of current face, and compare their deepness
				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fyz[i] -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (27).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i] -1, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXY, border_faces, va->tab_fyz[i] +ps -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (28).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXY, border_faces, va->tab_fyz[i]+ps-1, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fyz[i] -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (29).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i]-1, rs, ps)!=(v-1))
					break;

				v=cca_get_face_normaldepth(CC_FXZ, border_faces, va->tab_fyz[i] +rs -1, rs, ps);
				if(v==0)
				{
					fprintf(stderr, "complexe compute_vise_surfacic_axis(): cca_get_face_rvsa_depth failed (30).\n");
					return(NULL);
				}
				v--;
				if(v+alpha<=val && cca_get_face_rvsa_depth(CC_FXZ, border_faces, va->tab_fyz[i]+rs-1, rs, ps)!=(v-1))
					break;
			}

			//If we arrive here, it means the current face belongs to VSA
			if( complexe_add_element(result, va->tab_fyz[i], CC_FYZ) <0)
			{
				fprintf(stderr, "complexe compute_vise_surfacic_axis(): complexe_add_element failed (3).\n");
				return(NULL);
			}
		}
	}

	complexe_free_complexe(va);

	return result;
}


complexe *cca_geodilat_element_to_complex(face_desc* data, struct xvimage *image, uint32_t rs, uint32_t ps, uint32_t N)
{
	list *elements, *temp1, *temp2;
	face_desc *e, *h;
	complexe *result;

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cca_geodilat_element_to_complex(): Error, could not build new complexe\n");
		return(NULL);
	}

	elements=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 1024);
	if(elements==NULL)
	{
		fprintf(stderr, "cca_geodilat_element_to_complex(): Memory allocation error(1).\n");
		return(NULL);
	}

	list_push_pointer(elements, data);
	UCHARDATA(image)[data->pixnumber] &= (255-data->face_type);

	while(!list_isempty(elements))
	{
		e=(face_desc*)list_pop_pointer(elements);

		complexe_add_element(result, e->pixnumber, e->face_type);

		temp1=cca_list_starface_dimension_direct_over(e->pixnumber, e->face_type, rs, ps/rs, N/ps);
		temp2=cca_list_subfaces_dimension_direct_under(e->pixnumber, e->face_type, rs, ps);

		if(temp1==NULL || temp2==NULL)
		{
			fprintf(stderr, "cca_geodilat_element_to_complex(): Error in subfunction.\n");
			return(NULL);
		}

		list_append_to_other_list(temp2, temp1);
		list_delete(temp2, NO_FREE_DATA);

		while(!list_isempty(temp1))
		{
			h=(face_desc*)list_pop_pointer(temp1);
			if(	(UCHARDATA(image)[h->pixnumber] & h->face_type) != 0)
			{
				list_push_pointer(elements, h);
				UCHARDATA(image)[h->pixnumber] &= (255-h->face_type);
			}
		}

		list_delete(temp1, NO_FREE_DATA);
	}

	list_delete(elements, NO_FREE_DATA);

	return(result);
}


complexe *cca_geodilat_simplecurve_to_complex(face_desc* data, struct xvimage *image, uint32_t rs, uint32_t ps, uint32_t N)
{
	list *elements, *temp1, *temp2;
	face_desc *e, *h, save;
	complexe *result;

	e=NULL;

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cca_geodilat_element_to_complex(): Error, could not build new complexe\n");
		return(NULL);
	}

	elements=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 512);
	if(elements==NULL)
	{
		fprintf(stderr, "cca_geodilat_element_to_complex(): Memory allocation error(1).\n");
		return(NULL);
	}


	if( (UCHARDATA(image)[data->pixnumber] & CC_PT) == 0)
	{
		complexe_add_element(result, data->pixnumber, CC_PT);
		save.pixnumber=data->pixnumber;
	}
	else
	{
		switch(data->face_type)
		{
			case(CC_AX):
				complexe_add_element(result, data->pixnumber+1, CC_PT);
				save.pixnumber=data->pixnumber+1;
				break;

			case(CC_AY):
				complexe_add_element(result, data->pixnumber+rs, CC_PT);
				save.pixnumber=data->pixnumber+rs;
				break;

			default:
				complexe_add_element(result, data->pixnumber+ps, CC_PT);
				save.pixnumber=data->pixnumber+ps;
				break;
		}
	}

	list_push_pointer(elements, data);
	UCHARDATA(image)[data->pixnumber] &= (255-data->face_type);

	while(!list_isempty(elements))
	{
		e=(face_desc*)list_pop_pointer(elements);

		if(e->face_type==CC_PT)
		{
			complexe_add_element(result, e->pixnumber, CC_PT);
			save.pixnumber=e->pixnumber;
		}

		temp1=cca_list_starface_dimension_direct_over(e->pixnumber, e->face_type, rs, ps/rs, N/ps);
		temp2=cca_list_subfaces_dimension_direct_under(e->pixnumber, e->face_type, rs, ps);

		if(temp1==NULL || temp2==NULL)
		{
			fprintf(stderr, "cca_geodilat_element_to_complex(): Error in subfunction.\n");
			return(NULL);
		}

		list_append_to_other_list(temp2, temp1);
		list_delete(temp2, NO_FREE_DATA);

		while(!list_isempty(temp1))
		{
			h=(face_desc*)list_pop_pointer(temp1);
			if(	(UCHARDATA(image)[h->pixnumber] & h->face_type) != 0)
			{
				list_push_pointer(elements, h);
				UCHARDATA(image)[h->pixnumber] &= (255-h->face_type);
			}
		}

		list_delete(temp1, NO_FREE_DATA);
	}



	if( e->pixnumber != save.pixnumber)
		complexe_add_element(result, e->pixnumber, CC_PT);
	else
	{
		switch(e->face_type)
		{
			case(CC_AX):
				complexe_add_element(result, e->pixnumber+1, CC_PT);
				break;

			case(CC_AY):
				complexe_add_element(result, e->pixnumber+rs, CC_PT);
				break;

			default:
				complexe_add_element(result, e->pixnumber+ps, CC_PT);
				break;
		}
	}

	list_delete(elements, NO_FREE_DATA);

	return(result);
}


complexe* cca_geodilat_to_complexe(struct xvimage *image, face_desc *f)
{
	complexe *result;
	list *l;
	face_desc* r;
	uint32_t rs, ps;

	rs=rowsize(image);
	ps=colsize(image)*rs;

	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cca_geodilate_to_complexe(): Could not create output complexe.\n");
		return(NULL);
	}

	l=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 1024);
	if(l==NULL)
	{
		fprintf(stderr, "cca_geodilate_to_complexe(): Could not create list.\n");
		return(NULL);
	}

	list_push_pointer(l, f);
	UCHARDATA(image)[f->pixnumber] = (UCHARDATA(image)[f->pixnumber] | f->face_type) - f->face_type;
	while(!list_isempty(l))
	{
		r=(face_desc*)list_pop_pointer(l);
		complexe_add_element(result, r->pixnumber, r->face_type);
		cca_move_starfaces_to_list(image, l, r, rs, ps);
		cca_move_subfaces_to_list(image, l, r, rs, ps);
	}

	list_delete(l, NO_FREE_DATA);
	return result;
}




int32_t cca_geodilat(struct xvimage *image, struct xvimage *container, struct xvimage *avoid)
{
	list *elements, *temp;
	uint32_t rs, ps, N, cs, d, i, j;
	face_desc g, *e, *h;
	unsigned char f;

	rs=rowsize(image);
	cs=colsize(image);
	ps=cs*rs;
	d=depth(image);
	N=d*ps;

	elements=list_newlist_dynam(MODE_FIFO_DYN, sizeof(face_desc), 1024);
	if(elements==NULL)
	{
		fprintf(stderr, "cca_geodilat(): Memory allocation error(1).\n");
		return(-1);
	}

	for(i=0; i<N; i++)
	{
		f=CC_PT;
		for(j=0; j<8; j++)
		{
			if( ((UCHARDATA(image)[i] & f) != 0)  &&
				((avoid==NULL) || ((UCHARDATA(avoid)[i] & f) == 0)) )
			{
				g.pixnumber=i;
				g.face_type=f;
				list_push_pointer(elements, &g);
			}
			f=f*2;
		}
	}

	while(!list_isempty(elements))
	{
		e=(face_desc*)list_pop_pointer(elements);

		temp=cca_list_starface_dimension_direct_over(e->pixnumber, e->face_type, rs, cs, d);
		while(!list_isempty(temp))
		{
			h=(face_desc*)list_pop_pointer(temp);
			if(	((UCHARDATA(image)[h->pixnumber] & h->face_type) == 0)  &&
				((UCHARDATA(container)[h->pixnumber] & h->face_type) != 0) &&
				((avoid==NULL) || ((UCHARDATA(avoid)[h->pixnumber] & h->face_type) == 0)) )
			{
				g.pixnumber=h->pixnumber;
				g.face_type=h->face_type;
				list_push_pointer(elements, &g);
				UCHARDATA(image)[h->pixnumber] |= h->face_type;
			}
		}

		list_delete(temp, NO_FREE_DATA);

		temp=cca_list_subfaces_dimension_direct_under(e->pixnumber, e->face_type, rs, ps);
		while(!list_isempty(temp))
		{
			h=(face_desc*)list_pop_pointer(temp);
			if(	((UCHARDATA(image)[h->pixnumber] & h->face_type) == 0)  &&
				((UCHARDATA(container)[h->pixnumber] & h->face_type) != 0) &&
				((avoid==NULL) || ((UCHARDATA(avoid)[h->pixnumber] & h->face_type) == 0)) )
			{
				g.pixnumber=h->pixnumber;
				g.face_type=h->face_type;
				list_push_pointer(elements, &g);
				UCHARDATA(image)[h->pixnumber] |= h->face_type;
			}
		}

		list_delete(temp, NO_FREE_DATA);
	}

	list_delete(elements, NO_FREE_DATA);

	return(0);
}



list **cca_link_surfaces_to_border_edges(list *surfaces, list *border_edges, uint32_t rs, uint32_t ps, uint32_t N)
{
	list **temp, **result;
	uint32_t i, k;

	temp=cca_link_border_edges_to_surfaces(surfaces, border_edges, rs, ps, N);
	if(temp==NULL)
	{
		fprintf(stderr, "cca_link_surfaces_to_border_edges(): Error in subfunction.\n");
		return(NULL);
	}

	result=(list**)calloc(list_getsize(surfaces), sizeof(list*));
	if(result==NULL)
	{
		fprintf(stderr,"ccca_link_surfaces_to_border_edges(): Memory allocation error(2).\n");
		return(NULL);
	}
	for(i=0; i<surfaces->size; i++)
	{
		result[i]=list_newlist(MODE_FIFO, 512);
		if(result[i]==NULL)
		{
			fprintf(stderr,"cca_link_surfaces_to_border_edges(): Memory allocation error(3).\n");
			return(NULL);
		}
	}

	for(i=0; i<border_edges->size; i++)
	{
		while(!list_isempty(temp[i]))
		{
			k=list_pop_uint32_t(temp[i]);
			list_push_uint32_t(result[k], i);
		}
	}

	for(i=0; i<border_edges->size; i++)
		list_delete(temp[i], NO_FREE_DATA);

	free(temp);

	return(result);
}



//Link simple surfaces with the border edges (decomposed as curves)
//Result given as an array of list : for each curve, we have list of surfaces it belongs to
list **cca_link_border_edges_to_surfaces(list *surfaces, list *border_edges, uint32_t rs, uint32_t ps, uint32_t N)
{
	uint32_t i, j, k, *surf_array, a, *tab_edge[3], num_edge[3], num_surf;
	complexe *t;
	unsigned char *surfaces_taken, f;
	list **result, *cont;
	face_desc *h;


	//************************************
	//Construction of array of faces
	//************************************

	//Allocate the array which will give for each face the surface number it belongs to
	surf_array=(uint32_t*)calloc(N, 3*sizeof(uint32_t));
	if(surf_array==NULL)
	{
		fprintf(stderr,"cca_link_border_edges_to_surfaces(): Memory allocation error.\n");
		return(NULL);
	}

	for(i=0; i<surfaces->size; i++)
	{
		t=(complexe*)list_pop_pointer(surfaces);
		k=0;
		for(j=0; j<t->num_fxy; j++)
			surf_array[3*(t->tab_fxy[j]) + k]=i+1;
		k++;
		for(j=0; j<t->num_fxz; j++)
			surf_array[3*(t->tab_fxz[j]) + k]=i+1;
		k++;
		for(j=0; j<t->num_fyz; j++)
			surf_array[3*(t->tab_fyz[j]) + k]=i+1;
		list_push_pointer(surfaces, t);
	}



	//**************************************
	//Scan the edges, and find the surfaces they belong to
	//**************************************
	//The array of list which will be used for the result
	result=(list**)calloc(list_getsize(border_edges), sizeof(list*));
	if(result==NULL)
	{
		fprintf(stderr,"cca_link_border_edges_to_surfaces(): Memory allocation error(2).\n");
		return(NULL);
	}
	for(i=0; i<border_edges->size; i++)
	{
		result[i]=list_newlist(MODE_FIFO, 512);
		if(result[i]==NULL)
		{
			fprintf(stderr,"cca_link_border_edges_to_surfaces(): Memory allocation error(3).\n");
			return(NULL);
		}
	}

	//For each curve, we save the surfaces which were already taken into account
	surfaces_taken=(unsigned char*)calloc(list_getsize(surfaces), sizeof(unsigned char));
	if(surfaces_taken==NULL)
	{
		fprintf(stderr,"cca_link_border_edges_to_surfaces(): Memory allocation error(4).\n");
		return(NULL);
	}


	//Scan each curve
	for(i=0; i<border_edges->size; i++)
	{
		memset(surfaces_taken, 0, list_getsize(surfaces)*sizeof(unsigned char));
		t=(complexe*)list_pop_pointer(border_edges);
		num_edge[0]=t->num_az; num_edge[1]=t->num_ay; num_edge[2]=t->num_ax;
		tab_edge[0]=t->tab_az; tab_edge[1]=t->tab_ay; tab_edge[2]=t->tab_ax;
		f=CC_AZ;
		for(k=0; k<3; k++)
		{
			//Scan each kind of edge from our curve
			for(j=0; j<num_edge[k]; j++)
			{
				cont=cca_list_starface_dimension_direct_over(tab_edge[k][j], f, rs, ps/rs, N/ps);
				if(cont==NULL)
				{
					fprintf(stderr, "cca_link_border_edges_to_surfaces(): Error in subfunction.\n");
					return(NULL);
				}

				while(!list_isempty(cont))
				{
					h=(face_desc*)list_pop_pointer(cont);
					if(h->face_type==CC_FXY) a=0;
					else if (h->face_type==CC_FXZ) a=1;
					else if (h->face_type==CC_FYZ) a=2;
					else
					{
						fprintf(stderr, "cca_link_border_edges_to_surfaces(): Wrong container found for edge.\n");
						return(NULL);
					}

					num_surf=surf_array[3*(h->pixnumber)+a];
					if(num_surf!=0 && surfaces_taken[num_surf-1]==0)
					{
						list_push_uint32_t(result[i], (num_surf-1));
						surfaces_taken[num_surf-1]=1;
					}
				}

				list_delete(cont, NO_FREE_DATA);
			}
			f=f*2;
		}
		list_push_pointer(border_edges, t);
	}


	//**************************************
	//End
	//**************************************
	free(surfaces_taken);
	free(surf_array);

	return(result);
}


//result must be allocated and be a three double element array
uint32_t does_Line_Intersect_With_Face(uint32_t pt_face, unsigned char type_face, uint32_t rs, uint32_t ps, double* pt_space, double *direction, double *result)
{
	double dx, dy, dz, alpha;

	switch(type_face)
	{
		//We compute the intersection with the plan z=c;
		case(CC_FXY):
			//First, check if the direction is parallel to the plan... If the point is already on the face, don't compute anything
			dz=pt_space[2]-(double)getzfrompixnum(pt_face, rs, ps);
			if(fabs(dz) <= EPSILON)
			{
				//Yes, the point is already on same plan than face, check if it is inside the face
				dx=pt_space[0]-(double)getxfrompixnum(pt_face, rs, ps);
				dy=pt_space[1]-(double)getyfrompixnum(pt_face, rs, ps);
				if(dx>=0 && dx<=1.0 && dy>=0 && dy<=1.0)
				{
					result[0]=pt_space[0];
					result[1]=pt_space[1];
					result[2]=pt_space[2];
					return(1);
				}
				return(0);
			}
			//Else, check if the projection direction is compatible with the plan
			if(fabs(direction[2])<EPSILON)
				return(0);

			//Everything is fine, compute the intersection point (coordinates x and y)
			alpha=((double)getzfrompixnum(pt_face, rs, ps) - pt_space[2]) / direction[2];
			result[0]=pt_space[0]+alpha*direction[0];
			result[1]=pt_space[1]+alpha*direction[1];
			result[2]=(double)getzfrompixnum(pt_face, rs, ps);

			//Check if the point is inside the face
			dx=result[0]-(double)getxfrompixnum(pt_face, rs, ps);
			dy=result[1]-(double)getyfrompixnum(pt_face, rs, ps);
			if(dx>=0 && dx<=1.0 && dy>=0 && dy<=1.0)
				return(1);
			else
				return(0);
			break;



		case(CC_FYZ):
			//First, check if the direction is parallel to the plan... If the point is already on the face, don't compute anything
			dx=pt_space[0]-(double)getxfrompixnum(pt_face, rs, ps);
			if(fabs(dx) <= EPSILON)
			{
				//Yes, the point is already on same plan than face, check if it is inside the face
				dz=pt_space[2]-(double)getzfrompixnum(pt_face, rs, ps);
				dy=pt_space[1]-(double)getyfrompixnum(pt_face, rs, ps);
				if(dz>=0 && dz<=1.0 && dy>=0 && dy<=1.0)
				{
					result[0]=pt_space[0];
					result[1]=pt_space[1];
					result[2]=pt_space[2];
					return(1);
				}
				return(0);
			}
			//Else, check if the projection direction is compatible with the plan
			if(fabs(direction[0])<EPSILON)
				return(0);

			//Everything is fine, compute the intersection point (coordinates x and y)
			alpha=((double)getxfrompixnum(pt_face, rs, ps) - pt_space[0]) / direction[0];
			result[2]=pt_space[2]+alpha*direction[2];
			result[1]=pt_space[1]+alpha*direction[1];
			result[0]=(double)getxfrompixnum(pt_face, rs, ps);

			//Check if the point is inside the face
			dz=result[2]-(double)getzfrompixnum(pt_face, rs, ps);
			dy=result[1]-(double)getyfrompixnum(pt_face, rs, ps);
			if(dz>=0 && dz<=1.0 && dy>=0 && dy<=1.0)
				return(1);
			else
				return(0);
			break;


		case(CC_FXZ):
			//First, check if the direction is parallel to the plan... If the point is already on the face, don't compute anything
			dy=pt_space[1]-(double)getyfrompixnum(pt_face, rs, ps);
			if(fabs(dy) <= EPSILON)
			{
				//Yes, the point is already on same plan than face, check if it is inside the face
				dx=pt_space[0]-(double)getxfrompixnum(pt_face, rs, ps);
				dz=pt_space[2]-(double)getzfrompixnum(pt_face, rs, ps);
				if(dx>=0 && dx<=1.0 && dz>=0 && dz<=1.0)
				{
					result[0]=pt_space[0];
					result[1]=pt_space[1];
					result[2]=pt_space[2];
					return(1);
				}
				return(0);
			}
			//Else, check if the projection direction is compatible with the plan
			if(fabs(direction[1])<EPSILON)
				return(0);

			//Everything is fine, compute the intersection point (coordinates x and y)
			alpha=((double)getyfrompixnum(pt_face, rs, ps) - pt_space[1]) / direction[1];
			result[0]=pt_space[0]+alpha*direction[0];
			result[2]=pt_space[2]+alpha*direction[2];
			result[1]=(double)getyfrompixnum(pt_face, rs, ps);

			//Check if the point is inside the face
			dx=result[0]-(double)getxfrompixnum(pt_face, rs, ps);
			dz=result[2]-(double)getzfrompixnum(pt_face, rs, ps);
			if(dx>=0 && dx<=1.0 && dz>=0 && dz<=1.0)
				return(1);
			else
				return(0);
			break;


		default:
			fprintf(stderr, "does_Line_Intersect_With_Face(): Warning, unknown face type.\n");
			return (0);
	}
}


uint32_t complexe_get_dimension(complexe* cpl)
{
	if(cpl==NULL) return (0);
	else if( cpl->num_vol > 0) return(3);
	else if( (cpl->num_fxy + cpl->num_fxz + cpl->num_fyz) > 0) return(2);
	else if( (cpl->num_ax + cpl->num_ay + cpl->num_az) > 0) return(1);
	else return(0);
}


//Similar to complexe_count_elements_inside_cca(complexe *cpl, struct xvimage* image), but quicker as it stops scan as soon as one element is found
uint32_t complexe_is_partly_inside_cca(complexe *cpl, struct xvimage *image)
{
	uint32_t i;
	//We look only at the highest dimension faces of the complex
	for(i=0; i<cpl->num_vol; i++)
		if( (UCHARDATA(image)[cpl->tab_vol[i]] & CC_VOL) != 0) return(TRUE);

	for(i=0; i<cpl->num_fxy; i++)
		if( (UCHARDATA(image)[cpl->tab_fxy[i]] & CC_FXY) != 0) return(TRUE);

	for(i=0; i<cpl->num_fxz; i++)
		if( (UCHARDATA(image)[cpl->tab_fxz[i]] & CC_FXZ) != 0) return(TRUE);

	for(i=0; i<cpl->num_fyz; i++)
		if( (UCHARDATA(image)[cpl->tab_fyz[i]] & CC_FYZ) != 0) return(TRUE);

	for(i=0; i<cpl->num_ax; i++)
		if( (UCHARDATA(image)[cpl->tab_ax[i]] & CC_AX) != 0) return(TRUE);

	for(i=0; i<cpl->num_ay; i++)
		if( (UCHARDATA(image)[cpl->tab_ay[i]] & CC_AY) != 0) return(TRUE);

	for(i=0; i<cpl->num_az; i++)
		if( (UCHARDATA(image)[cpl->tab_az[i]] & CC_AZ) != 0) return(TRUE);

	for(i=0; i<cpl->num_pt_obj; i++)
		if( (UCHARDATA(image)[cpl->tab_pt_obj[i]] & CC_PT) != 0) return(TRUE);

	return(FALSE);
}

void complexe_keep_highest_dimension_faces(complexe *cpl)
{
	uint32_t d=complexe_get_dimension(cpl);

	switch(d)
	{
		case(3):
			if(cpl->num_fxy > 0) {free(cpl->tab_fxy); cpl->tab_fxy=NULL; cpl->num_fxy=0;}
			if(cpl->num_fxz > 0) {free(cpl->tab_fxz); cpl->tab_fxz=NULL; cpl->num_fxz=0;}
			if(cpl->num_fyz > 0) {free(cpl->tab_fyz); cpl->tab_fyz=NULL; cpl->num_fyz=0;}

		case(2):
			if(cpl->num_ax > 0) {free(cpl->tab_ax); cpl->tab_ax=NULL; cpl->num_ax=0;}
			if(cpl->num_ay > 0) {free(cpl->tab_ay); cpl->tab_ay=NULL; cpl->num_ay=0;}
			if(cpl->num_az > 0) {free(cpl->tab_az); cpl->tab_az=NULL; cpl->num_az=0;}

		case(1):
			if(cpl->num_pt_obj > 0) {free(cpl->tab_pt_obj); cpl->tab_pt_obj=NULL; cpl->num_pt_obj=0;}

		default:
			break;
	}
}


uint32_t complexe_count_faces_inside_cca(complexe *cpl, struct xvimage* image)
{
	uint32_t i, r;

	r=0;
	//We look only at the highest dimension faces of the complex
	for(i=0; i<cpl->num_vol; i++)
		if( (UCHARDATA(image)[cpl->tab_vol[i]] & CC_VOL) != 0) r++;

	for(i=0; i<cpl->num_fxy; i++)
		if( (UCHARDATA(image)[cpl->tab_fxy[i]] & CC_FXY) != 0) r++;

	for(i=0; i<cpl->num_fxz; i++)
		if( (UCHARDATA(image)[cpl->tab_fxz[i]] & CC_FXZ) != 0) r++;

	for(i=0; i<cpl->num_fyz; i++)
		if( (UCHARDATA(image)[cpl->tab_fyz[i]] & CC_FYZ) != 0) r++;

	for(i=0; i<cpl->num_ax; i++)
		if( (UCHARDATA(image)[cpl->tab_ax[i]] & CC_AX) != 0) r++;

	for(i=0; i<cpl->num_ay; i++)
		if( (UCHARDATA(image)[cpl->tab_ay[i]] & CC_AY) != 0) r++;

	for(i=0; i<cpl->num_az; i++)
		if( (UCHARDATA(image)[cpl->tab_az[i]] & CC_AZ) != 0) r++;

	for(i=0; i<cpl->num_pt_obj; i++)
		if( (UCHARDATA(image)[cpl->tab_pt_obj[i]] & CC_PT) != 0) r++;

	return(r);
}


uint32_t complexe_get_num_faces(complexe *cpl)
{
	return(cpl->num_pt_obj + cpl->num_ax + cpl->num_ay + cpl->num_az + cpl->num_fxy + cpl->num_fxz + cpl->num_fyz + cpl->num_vol);
}


