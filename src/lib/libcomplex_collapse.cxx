//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>
#include <mcimage.h>

//Librairie perso
#include <libcomplex.h>



uint32_t cca_is_opposite_face_free_or_vise(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, unsigned char face_contained, unsigned char direction_free, uint32_t rs, uint32_t ps)
{
	uint32_t newpixnum;
	switch(face_contained)
	{
		case(CC_FXY):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					newpixnum=pixnumber+ps;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum+ps, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				case(CC_FACE_COLL_DOWN):
					newpixnum=pixnumber-ps;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum-ps, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(1).\n");
					break;
			}
			break;

		case(CC_FXZ):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					newpixnum=pixnumber+rs;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum+rs, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				case(CC_FACE_COLL_DOWN):
					newpixnum=pixnumber-rs;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum-rs, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(2).\n");
					break;
			}
			break;

		case(CC_FYZ):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					newpixnum=pixnumber+1;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum+1, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				case(CC_FACE_COLL_DOWN):
					newpixnum=pixnumber-1;
					if(cca_is_face_onborder(cca_image, face_contained, getxfrompixnum(newpixnum, rs, ps), getyfrompixnum(newpixnum, rs, ps), getzfrompixnum(newpixnum, rs, ps))!=0)
					{
						return(	cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN );
					}
					else
					{
						return(	(cca_is_face_free(cca_image, inhibit, newpixnum, rs, ps, face_contained)==CC_FACE_COLL_DOWN) ||
								(cca_is_face_free(cca_image, inhibit, newpixnum-1, rs, ps, face_contained)==CC_FACE_COLL_DOWN)	);
					}
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(3).\n");
					break;
			}
			break;

		default:
			return(cca_is_opposite_face_free(cca_image, inhibit, pixnumber, face_contained, direction_free, rs, ps));
			break;
	}
	assert(0);
	return(0);

}

int32_t cca_get_collapse_directions_of_face(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, unsigned char face, uint32_t rs, uint32_t ps, char *result)
{
	uint32_t i;
	switch(face)
	{
		//Results: CC_FXY UP/DOWN - CC_FXZ UP/DOWN - CC_FYZ UP/DOWN
		case(CC_VOL):
			for(i=0; i<6; i++) result[i]=0;
			if( (UCHARDATA(cca_image)[pixnumber]&CC_VOL)==0) return(0);
			if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY)))
				result[0]=1;
			if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY)))
				result[1]=1;
			if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ)))
				result[2]=1;
			if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ)))
				result[3]=1;
			if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ)))
				result[4]=1;
			if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ)))
				result[5]=1;
			return(0);
			break;

		default:
			fprintf(stderr, "cca_get_collapse_directions_of_face(): Error, face not yet implemented.\n");
			return(-1);
	}

	return(-1);
}


collapsedir cca_get_best_collapse_direction(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, unsigned char face_container, uint32_t rs, uint32_t ps)
{
	uint32_t x, y, z, xdeb, ydeb, zdeb, xend, yend, zend, i, indmax;
	unsigned char result_local[6], result[6];
	collapsedir res;
	switch(face_container)
	{
		case(CC_VOL):
			x=getxfrompixnum(pixnumber, rs, ps);
			y=getyfrompixnum(pixnumber, rs, ps);
			z=getzfrompixnum(pixnumber, rs, ps);
			xdeb=x; xend=x;
			if(x!=0) xdeb=x-1;
			if(x!=rs-1) xend=x+1;
			ydeb=y; yend=y;
			if(y!=0) ydeb=y-1;
			if((int32_t)y!=colsize(cca_image)-1) yend=y+1;
			zdeb=z; zend=z;
			if(z!=0) zdeb=z-1;
			if((int32_t)z!=depth(cca_image)-1) zend=z+1;

			for(i=0; i<6; i++)
				result[i]=0;

			for(z=zdeb; z<=zend; z++)
			for(y=ydeb; y<=yend; y++)
			for(x=xdeb; x<=xend; x++)
			{
				cca_get_collapse_directions_of_face(cca_image, inhibit, x+y*rs+z*ps, CC_VOL, rs, ps, (char*)result_local);
				if(result_local[0]==1) result[0]+=1-result_local[1];
				if(result_local[1]==1) result[1]+=1-result_local[0];
				if(result_local[2]==1) result[2]+=1-result_local[3];
				if(result_local[3]==1) result[3]+=1-result_local[2];
				if(result_local[4]==1) result[4]+=1-result_local[5];
				if(result_local[5]==1) result[5]+=1-result_local[4];
			}

			indmax=0;
			for(i=1; i<6; i++)
			{
				if(result[indmax] < result[i])
					indmax=i;
			}

			switch(indmax)
			{
				case(0):
					res.face=CC_FXY;
					res.direction=CC_FACE_COLL_UP;
					break;

				case(1):
					res.face=CC_FXY;
					res.direction=CC_FACE_COLL_DOWN;
					break;

				case(2):
					res.face=CC_FXZ;
					res.direction=CC_FACE_COLL_UP;
					break;

				case(3):
					res.face=CC_FXZ;
					res.direction=CC_FACE_COLL_DOWN;
					break;

				case(4):
					res.face=CC_FYZ;
					res.direction=CC_FACE_COLL_UP;
					break;

				case(5):
					res.face=CC_FYZ;
					res.direction=CC_FACE_COLL_DOWN;
					break;

				default:
					res.face=EMPTY;
					res.direction=EMPTY;
					break;
			}

			return res;
			break;

		default:
			fprintf(stderr, "cca_get_best_collapse_direction(): Wrong face type.\n");
			res.face=EMPTY;
			res.direction=EMPTY;
			return res;
			break;
	}
}





//Says if, for a given pair (f,g) of free face, there exists e such that (f,e) free, and e \neq g and e and g parallel
uint32_t cca_is_opposite_face_free(struct xvimage *cca_image, struct xvimage* inhibit, uint32_t pixnumber, unsigned char face_contained, unsigned char direction_free, uint32_t rs, uint32_t ps)
{
	switch(face_contained)
	{
		case(CC_FXY):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, face_contained)==CC_FACE_COLL_DOWN);
					break;

				case(CC_FACE_COLL_DOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, face_contained)==CC_FACE_COLL_UP);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(1).\n");
					break;
			}
			break;

		case(CC_FXZ):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, face_contained)==CC_FACE_COLL_DOWN);
					break;

				case(CC_FACE_COLL_DOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, face_contained)==CC_FACE_COLL_UP);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(2).\n");
					break;
			}
			break;

		case(CC_FYZ):
			switch(direction_free)
			{
				case(CC_FACE_COLL_UP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, face_contained)==CC_FACE_COLL_DOWN);
					break;

				case(CC_FACE_COLL_DOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, face_contained)==CC_FACE_COLL_UP);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(3).\n");
					break;
			}
			break;

		case(CC_AX):
			switch(direction_free)
			{
				case(CC_EDGE_COLL_DIR1):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, face_contained)==CC_EDGE_COLL_DIR2);
					break;

				case(CC_EDGE_COLL_DIR2):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, face_contained)==CC_EDGE_COLL_DIR1);
					break;

				case(CC_EDGE_COLL_DIR3):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, face_contained)==CC_EDGE_COLL_DIR4);
					break;

				case(CC_EDGE_COLL_DIR4):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, face_contained)==CC_EDGE_COLL_DIR3);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(4).\n");
					break;
			}
			break;

		case(CC_AY):
			switch(direction_free)
			{
				case(CC_EDGE_COLL_DIR1):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, face_contained)==CC_EDGE_COLL_DIR2);
					break;

				case(CC_EDGE_COLL_DIR2):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, face_contained)==CC_EDGE_COLL_DIR1);
					break;

				case(CC_EDGE_COLL_DIR3):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, face_contained)==CC_EDGE_COLL_DIR4);
					break;

				case(CC_EDGE_COLL_DIR4):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, face_contained)==CC_EDGE_COLL_DIR3);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(5).\n");
					break;
			}
			break;

		case(CC_AZ):
			switch(direction_free)
			{
				case(CC_EDGE_COLL_DIR1):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, face_contained)==CC_EDGE_COLL_DIR2);
					break;

				case(CC_EDGE_COLL_DIR2):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, face_contained)==CC_EDGE_COLL_DIR1);
					break;

				case(CC_EDGE_COLL_DIR3):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, face_contained)==CC_EDGE_COLL_DIR4);
					break;

				case(CC_EDGE_COLL_DIR4):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, face_contained)==CC_EDGE_COLL_DIR3);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(6).\n");
					break;
			}
			break;

		case(CC_PT):
			switch(direction_free)
			{
				case(CC_VERT_COLL_XUP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, face_contained)==CC_VERT_COLL_XDOWN);
					break;

				case(CC_VERT_COLL_XDOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, face_contained)==CC_VERT_COLL_XUP);
					break;

				case(CC_VERT_COLL_YUP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, face_contained)==CC_VERT_COLL_YDOWN);
					break;

				case(CC_VERT_COLL_YDOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, face_contained)==CC_VERT_COLL_YUP);
					break;

				case(CC_VERT_COLL_ZUP):
					return(cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, face_contained)==CC_VERT_COLL_ZDOWN);
					break;

				case(CC_VERT_COLL_ZDOWN):
					return(cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, face_contained)==CC_VERT_COLL_ZUP);
					break;

				default:
					fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong direction given(7).\n");
					break;
			}
			break;

		default:
			fprintf(stderr, "cca_is_free_face_vise_free_in_given_direction(): Wrong face type given.\n");
			break;
	}
	assert(0);
	return(0);
}


uint32_t cca_is_free_face_total_vise_free(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, unsigned char facetype, uint32_t rs, uint32_t ps)
{
	uint32_t a,b,c,d,e,f;
	switch(facetype)
	{
		case(CC_VOL):
			a=ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY));
			b=ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY));
			c=ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ));
			d=ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ));
			e=ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ));
			f=ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ));
			return(	!(	(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY)) ^ ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY))) ||
						(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ)) ^ ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ))) ||
						(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ)) ^ ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ))) ));
			break;

		case(CC_FXY):
			return(	!(	(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)) ^ ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX))) ||
						(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)) ^ ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY))) ));
			break;

		case(CC_FYZ):
			return(	!(	(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)) ^ ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ))) ||
						(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)) ^ ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY))) ));
			break;

		case(CC_FXZ):
			return(	!(	(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)) ^ ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX))) ||
						(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)) ^ ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ))) ));
			break;

		case(CC_AX):
			return( !(	(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)) ^ ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT))) ));
			break;

		case(CC_AY):
			return( !(	(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)) ^ ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT))) ));
			break;

		case(CC_AZ):
			return( !(	(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)) ^ ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT))) ));
			break;

		default:
			fprintf(stderr, "cca_is_free_face_total_vise_free(): Wrong facetype given as parameter.\n");
			return(0);
			break;
	}
}



/*!	\brief Collapse a face in a cubical complex (no matter if the face is free or not, the test must be done before calling the function).

	\param cca_image The input image where the object is.
	\param pixnumber The array index which contains information about the face to collapse.
	\param rs Size of a row of the image.
	\param ps Size of a plane of the image.
	\param face_type Which face should we collapse (CC_FXY, CC_FXZ, CC_FYZ, CC_AX, CC_AY, CC_AZ, CC_PT).
	\param direction In which direction should we collapse (for a 2D face, CC_FACE_COLL_DOWN or CC_FACE_COLL_UP; for a 1D face, CC_EDGE_COLL_DIR1, CC_EDGE_COLL_DIR2, CC_EDGE_COLL_DIR3, CC_EDGE_COLL_DIR4; for a 0d face, CC_VERT_COLL_XUP, CC_VERT_COLL_XDOWN, CC_VERT_COLL_YUP, CC_VERT_COLL_YDOWN, CC_VERT_COLL_ZUP, CC_VERT_COLL_ZDOWN).

	\ingroup complex
	\ingroup collapse

	\author John Chaussard
*/
void cca_collapse_face(struct xvimage *cca_image, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction)
{
	//Remove the face from object
	UCHARDATA(cca_image)[pixnumber]&=(255-face_type);

	//And remove the container face from the object
	switch(face_type)
	{
		case(CC_FXY):
			if(direction==CC_FACE_COLL_UP)
				UCHARDATA(cca_image)[pixnumber]&=(255-CC_VOL);
			else
				UCHARDATA(cca_image)[pixnumber-ps]&=(255-CC_VOL);
			break;

		case(CC_FXZ):
			if(direction==CC_FACE_COLL_UP)
				UCHARDATA(cca_image)[pixnumber]&=(255-CC_VOL);
			else
				UCHARDATA(cca_image)[pixnumber-rs]&=(255-CC_VOL);
			break;

		case(CC_FYZ):
			if(direction==CC_FACE_COLL_UP)
				UCHARDATA(cca_image)[pixnumber]&=(255-CC_VOL);
			else
				UCHARDATA(cca_image)[pixnumber-1]&=(255-CC_VOL);
			break;

		case(CC_AX):
			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FXY);
					break;

				case(CC_EDGE_COLL_DIR2):
					UCHARDATA(cca_image)[pixnumber-rs]&=(255-CC_FXY);
					break;

				case(CC_EDGE_COLL_DIR3):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FXZ);
					break;

				case(CC_EDGE_COLL_DIR4):
					UCHARDATA(cca_image)[pixnumber-ps]&=(255-CC_FXZ);
					break;

				default:
					fprintf(stderr, "cca_collapse_face(): Warning, unknown direction given, no collapse performed (CC_AX).\n");
			}
			break;

		case(CC_AY):
			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FXY);
					break;

				case(CC_EDGE_COLL_DIR2):
					UCHARDATA(cca_image)[pixnumber-1]&=(255-CC_FXY);
					break;

				case(CC_EDGE_COLL_DIR3):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FYZ);
					break;

				case(CC_EDGE_COLL_DIR4):
					UCHARDATA(cca_image)[pixnumber-ps]&=(255-CC_FYZ);
					break;

				default:
					fprintf(stderr, "cca_collapse_face(): Warning, unknown direction given, no collapse performed (CC_AY).\n");
			}
			break;

		case(CC_AZ):
			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FXZ);
					break;

				case(CC_EDGE_COLL_DIR2):
					UCHARDATA(cca_image)[pixnumber-1]&=(255-CC_FXZ);
					break;

				case(CC_EDGE_COLL_DIR3):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_FYZ);
					break;

				case(CC_EDGE_COLL_DIR4):
					UCHARDATA(cca_image)[pixnumber-rs]&=(255-CC_FYZ);
					break;

				default:
					fprintf(stderr, "cca_collapse_face(): Warning, unknown direction given, no collapse performed (CC_AZ).\n");
			}
			break;

		case(CC_PT):
			switch(direction)
			{
				case(CC_VERT_COLL_XUP):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_AX);
					break;

				case(CC_VERT_COLL_XDOWN):
					UCHARDATA(cca_image)[pixnumber-1]&=(255-CC_AX);
					break;

				case(CC_VERT_COLL_YUP):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_AY);
					break;

				case(CC_VERT_COLL_YDOWN):
					UCHARDATA(cca_image)[pixnumber-rs]&=(255-CC_AY);
					break;

				case(CC_VERT_COLL_ZUP):
					UCHARDATA(cca_image)[pixnumber]&=(255-CC_AZ);
					break;

				case(CC_VERT_COLL_ZDOWN):
					UCHARDATA(cca_image)[pixnumber-ps]&=(255-CC_AZ);
					break;

				default:
					fprintf(stderr, "cca_collapse_face(): Warning, unknown direction given, no collapse performed (CC_PT).\n");
			}
			break;

		default:
		{
			fprintf(stderr, "cca_collapse_face(): Warning, unknown face type given, no collapse performed.\n");
		}
	}
}


int32_t cca_is_collapse_inside_face(unsigned char face_container_type, uint32_t face_container_pixnum, unsigned char face_collapse_type, uint32_t face_collapse_pixnum, unsigned char direction, uint32_t rs, uint32_t ps)
{
	switch(face_container_type | face_collapse_type)
	{
		case(CC_VOL | CC_FXY):
			return( ((direction==CC_FACE_COLL_UP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_FACE_COLL_DOWN) && ((face_container_pixnum+ps)==face_collapse_pixnum)) );

		case(CC_VOL | CC_FXZ):
			return( ((direction==CC_FACE_COLL_UP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_FACE_COLL_DOWN) && ((face_container_pixnum+rs)==face_collapse_pixnum)) );

		case(CC_VOL | CC_FYZ):
			return( ((direction==CC_FACE_COLL_UP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_FACE_COLL_DOWN) && ((face_container_pixnum+1)==face_collapse_pixnum)) );

		case(CC_VOL | CC_AX):
			if(direction<=CC_EDGE_COLL_DIR2)
				return(	cca_is_collapse_inside_face(CC_FXY, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FXY, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );
			else
				return(	cca_is_collapse_inside_face(CC_FXZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FXZ, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_VOL | CC_AY):
			if(direction<=CC_EDGE_COLL_DIR2)
				return(	cca_is_collapse_inside_face(CC_FXY, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FXY, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );
			else
				return(	cca_is_collapse_inside_face(CC_FYZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FYZ, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_VOL | CC_AZ):
			if(direction<=CC_EDGE_COLL_DIR2)
				return(	cca_is_collapse_inside_face(CC_FXZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FXZ, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );
			else
				return(	cca_is_collapse_inside_face(CC_FYZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_FYZ, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_VOL | CC_PT):
			if(direction<=CC_VERT_COLL_XDOWN)
				return( cca_is_collapse_inside_face(CC_AX, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AX, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AX, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AX, face_container_pixnum+rs+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

			else if(direction<=CC_VERT_COLL_YDOWN)
				return( cca_is_collapse_inside_face(CC_AY, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AY, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AY, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AY, face_container_pixnum+1+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

			else
				return( cca_is_collapse_inside_face(CC_AZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AZ, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AZ, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AZ, face_container_pixnum+rs+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_FXY | CC_AX):
			return( ((direction==CC_EDGE_COLL_DIR1) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR2) && ((face_container_pixnum+rs)==face_collapse_pixnum)) );

		case(CC_FXY | CC_AY):
			return( ((direction==CC_EDGE_COLL_DIR1) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR2) && ((face_container_pixnum+1)==face_collapse_pixnum)) );

		case(CC_FXY | CC_PT):
			if(direction<=CC_VERT_COLL_XDOWN)
				return(	cca_is_collapse_inside_face(CC_AX, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AX, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

			else
				return(	cca_is_collapse_inside_face(CC_AY, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AY, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_FXZ | CC_AX):
			return( ((direction==CC_EDGE_COLL_DIR3) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR4) && ((face_container_pixnum+ps)==face_collapse_pixnum)) );

		case(CC_FXZ | CC_AZ):
			return( ((direction==CC_EDGE_COLL_DIR1) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR2) && ((face_container_pixnum+1)==face_collapse_pixnum)) );

		case(CC_FXZ | CC_PT):
			if(direction<=CC_VERT_COLL_XDOWN)
				return(	cca_is_collapse_inside_face(CC_AX, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AX, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

			else
				return(	cca_is_collapse_inside_face(CC_AZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AZ, face_container_pixnum+1, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_FYZ | CC_AY):
			return( ((direction==CC_EDGE_COLL_DIR3) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR4) && ((face_container_pixnum+ps)==face_collapse_pixnum)) );

		case(CC_FYZ | CC_AZ):
			return( ((direction==CC_EDGE_COLL_DIR3) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_EDGE_COLL_DIR4) && ((face_container_pixnum+rs)==face_collapse_pixnum)) );

		case(CC_FYZ | CC_PT):
			if(direction<=CC_VERT_COLL_YDOWN)
				return(	cca_is_collapse_inside_face(CC_AY, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AY, face_container_pixnum+ps, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

			else
				return(	cca_is_collapse_inside_face(CC_AZ, face_container_pixnum, face_collapse_type, face_collapse_pixnum, direction, rs, ps) ||
						cca_is_collapse_inside_face(CC_AZ, face_container_pixnum+rs, face_collapse_type, face_collapse_pixnum, direction, rs, ps) );

		case(CC_AX | CC_PT):
			return(	((direction==CC_VERT_COLL_XUP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_VERT_COLL_XDOWN) && ((face_container_pixnum+1)==face_collapse_pixnum)) );

		case(CC_AY | CC_PT):
			return(	((direction==CC_VERT_COLL_YUP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_VERT_COLL_YDOWN) && ((face_container_pixnum+rs)==face_collapse_pixnum)) );

		case(CC_AZ | CC_PT):
			return(	((direction==CC_VERT_COLL_ZUP) && (face_container_pixnum==face_collapse_pixnum)) ||
					((direction==CC_VERT_COLL_ZDOWN) && ((face_container_pixnum+ps)==face_collapse_pixnum)) );
	}
	return(0);
}


int32_t cca_collapse_face_inside_container(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, uint32_t container_pixnumber, unsigned char container_facetype, list **free_faces, list **empty_lists)
{
	unsigned char dir, face;
	int32_t i, stop;
	uint32_t pix;
	face_desc t, r, s;

	//Collapse the face
	cca_collapse_face(cca_image, pixnumber, rs, ps, face_type, direction);

	//And collapse subfaces
	switch(face_type)
	{
		case(CC_FXY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY, dir, empty_lists);

			break;

		case(CC_FXZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, empty_lists);


			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX, dir, empty_lists);


			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ, dir, empty_lists);

			break;

		case(CC_FYZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY, dir, empty_lists);

			break;

		case(CC_AX):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT, dir, empty_lists);

			break;

		case(CC_AY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT, dir, empty_lists);

			break;

		case(CC_AZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, empty_lists);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT, dir, empty_lists);

			break;

		case(CC_PT):
			break;

		default:
			fprintf(stderr, "cca_collapse_face_inside_container(): Warning, unknown face type given, no collapse performed.\n");
			return(-1);

	}

	cca_check_free_faces_after_collapse(cca_image, inhibit, face_type, direction, empty_lists, pixnumber, rs, ps);

	face=CC_VOL;
	stop=0;
	t.face_type=container_facetype;
	t.pixnumber=container_pixnumber;
	while(stop==0)
	{
		stop=1;
		for(i=6; i>=0; i--)
		{
			face=face/2;
			while(!list_isempty(empty_lists[i]))
			{
				pix=list_pop_uint32_t(empty_lists[i]);
				dir=cca_is_face_free(cca_image, inhibit, pix, rs, ps, face);
				if(ISFREE(face, dir))
				{
					if(cca_is_collapse_inside_face(container_facetype, container_pixnumber, face, pix, dir, rs, ps))
					{
						//The face can be collapsed inside the container, so collapse it...
						stop=0;
						cca_collapse_face_and_subfaces(cca_image, inhibit, pix, rs, ps, face, dir, empty_lists);
					}
					else if(cca_get_face_dimension(container_facetype)<=2)
					{
						r=cca_get_container_of_free_face(face, pix, dir, rs, ps);
						s=cca_get_common_container(cca_image, t.face_type, t.pixnumber, r.face_type, r.pixnumber);
						if(s.face_type!=EMPTY && cca_get_face_dimension(s.face_type)<=(cca_get_face_dimension(container_facetype)+1))
						{
							stop=0;
							t.face_type=s.face_type;
							t.pixnumber=s.pixnumber;
							cca_collapse_face_and_subfaces(cca_image, inhibit, pix, rs, ps, face, dir, empty_lists);
						}

						else if(free_faces!=NULL)
							list_push_uint32_t(free_faces[i], pix);
					}
					else if(free_faces!=NULL)
						list_push_uint32_t(free_faces[i], pix);
				}
			}
		}
	}

	return(0);
}


int32_t cca_collapse_face_inside_container_with_two_images(struct xvimage *original_image, struct xvimage *collapsed_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, uint32_t container_pixnumber, unsigned char container_facetype, list **free_faces, list **empty_lists)
{
	unsigned char dir, face;
	int32_t i, stop;
	uint32_t pix;

	cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, face_type, direction, empty_lists);

	face=CC_VOL;
	stop=0;
	while(stop==0)
	{
		stop=1;
		for(i=6; i>=0; i--)
		{
			face=face/2;
			while(!list_isempty(empty_lists[i]))
			{
				pix=list_pop_uint32_t(empty_lists[i]);
				dir=cca_is_face_free(original_image, inhibit, pix, rs, ps, face);
				if(ISFREE(face, dir))
				{
					if(cca_is_collapse_inside_face(container_facetype, container_pixnumber, face, pix, dir, rs, ps))
					{
						//The face can be collapsed inside the container, so collapse it...
						stop=0;
						cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pix, rs, ps, face, dir, empty_lists);
					}
					else if(free_faces!=NULL)
						list_push_uint32_t(free_faces[i], pix);
				}
				else if(free_faces!=NULL)
				{
					list_push_uint32_t(free_faces[i], pix);
				}
			}
		}
	}

	return(0);
}


int32_t cca_collapse_face_and_subfaces(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, list **free_faces)
{
	unsigned char dir;

	//Collapse the face
	cca_collapse_face(cca_image, pixnumber, rs, ps, face_type, direction);

	//And collapse subfaces
	switch(face_type)
	{
		case(CC_FXY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY, dir, free_faces);

			break;

		case(CC_FXZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ, dir, free_faces);

			break;

		case(CC_FYZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY, dir, free_faces);

			break;

		case(CC_AX):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_AY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_AZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_PT):
			break;

		default:
			fprintf(stderr, "cca_collapse_face_and_subfaces(): Warning, unknown face type given, no collapse performed.\n");
			return(-1);

	}

	cca_check_free_faces_after_collapse(cca_image, inhibit, face_type, direction, free_faces, pixnumber, rs, ps);

	return(0);
}


int32_t cca_collapse_face_and_subfaces_with_two_images(struct xvimage *original_image, struct xvimage *collapsed_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, list **free_faces)
{
	unsigned char dir;

	//Collapse the face (we expect it to be free in both images)
	cca_collapse_face(original_image, pixnumber, rs, ps, face_type, direction);
	cca_collapse_face(collapsed_image, pixnumber, rs, ps, face_type, direction);

	//And collapse subfaces
	switch(face_type)
	{
		case(CC_FXY):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AY, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+rs, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+1, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+1, rs, ps, CC_AY, dir, free_faces);

			break;

		case(CC_FXZ):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+ps, rs, ps, CC_AX, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+1, rs, ps, CC_AZ, dir, free_faces);

			break;

		case(CC_FYZ):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_AY, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+rs, rs, ps, CC_AZ, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+ps, rs, ps, CC_AY, dir, free_faces);

			break;

		case(CC_AX):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+1, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+1, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_AY):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+rs, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_AZ):
			dir=cca_is_face_free(original_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber, rs, ps, CC_PT, dir, free_faces);

			dir=cca_is_face_free(original_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces_with_two_images(original_image, collapsed_image, inhibit, pixnumber+ps, rs, ps, CC_PT, dir, free_faces);

			break;

		case(CC_PT):
			break;

		default:
			fprintf(stderr, "cca_collapse_face_and_subfaces_with_two_images(): Warning, unknown face type given, no collapse performed.\n");
			return(-1);

	}

	cca_check_free_faces_after_collapse(collapsed_image, inhibit, face_type, direction, free_faces, pixnumber, rs, ps);

	return(0);
}


int32_t cca_collapse_face_and_subfaces2(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t pixnumber, uint32_t rs, uint32_t ps, unsigned char face_type, unsigned char direction, uint32_t container_pixnumber, unsigned char container_facetype, list **free_faces)
{
	unsigned char dir;

/*	for(i=0; i<7; i++)
	{
		ff[i] = list_newlist(MODE_FIFO);
		if(ff[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_face_and_subfaces: Error, could not create new list.\n");
			return(-1);
		}
	}*/


	//Collapse the face
	cca_collapse_face(cca_image, pixnumber, rs, ps, face_type, direction);

	//And collapse subfaces
	switch(face_type)
	{
		case(CC_FXY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_FXZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AX, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_FYZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AZ, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_AY, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_AX):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_AY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_AZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_collapse_face_and_subfaces2(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT, dir, container_pixnumber, container_facetype, free_faces);

			break;

		case(CC_PT):

			break;

		default:
			fprintf(stderr, "cca_collapse_face_and_subfaces(): Warning, unknown face type given, no collapse performed.\n");
			return(-1);

	}

	cca_check_free_faces_after_collapse(cca_image, inhibit, face_type, direction, free_faces, pixnumber, rs, ps);

	//Scan the resulting free faces, and try to collapse them if they can collapse inside the current container
	/*face=CC_FYZ;
	for(i=6; i>=0; i--)
	{
		while(!list_isempty(ff[i]))
		{
			pix=list_pop_uint32_t(ff[i]);
			dir=cca_is_face_free(cca_image, inhibit, pix, rs, ps, face);
			if(ISFREE(face, dir))
			{
				if( is_collapse_inside_face(container_facetype, container_pixnumber, face, pix, dir, rs, ps)!=0)
				{
					cca_collapse_face_and_subfaces(cca_image, inhibit, pix, rs, ps, face, dir, container_pixnumber, container_facetype, free_faces);
				}
				else
				{
					list_push(free_faces[i], (void*)pix);
				}
			}
		}
		list_delete(ff[i], NO_FREE_DATA);
		face=face/2;
	}*/

	return(0);
}


/*!	\brief Collapse algorithm: collapse an object until no more free faces can be found in the object.

	\param cca_image The input image where the object is.
	\param inhibit The inhibit image containing the faces to keep in the final result. Can be set to NULL.

	\ingroup complex
	\ingroup collapse

	\author John Chaussard
*/
int32_t cca_collapse(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop)
{
	list *free_faces[7]; //On list per face type...
	list *free_faces_bis[7];
	uint32_t N, j, k, l, rs, cs, d, ps, t, count;
	int32_t i;
	unsigned char face, stop;

	count=0;

	//Prepare and initialize the lists which will be used...
	for(i=0; i<7; i++)
	{
		free_faces[i] = list_newlist(MODE_FIFO, 1024);
		if(free_faces[i]==NULL)
		{
			fprintf(stderr, "cca_collapse(): Error, could not create new list.\n");
			return(-1);
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;


	/*****************************************
	First scan, we scan the whole image in order to
	detect the free faces...
	*******************************************/
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<(int32_t)rs; i++)
			{
				if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_PT)))
					list_push_uint32_t(free_faces[0], l);

				if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ)))
					list_push_uint32_t(free_faces[1], l);

				if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY)))
					list_push_uint32_t(free_faces[2], l);

				if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX)))
					list_push_uint32_t(free_faces[3], l);

				if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
					list_push_uint32_t(free_faces[4], l);

				if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
					list_push_uint32_t(free_faces[5], l);

				if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
					list_push_uint32_t(free_faces[6], l);

				l++;
			}


	/***********************************
	Second phase, collapse free faces
	**********************************/
	stop=0;
	while(stop==0 && (numloop==0 || count<numloop))
	{
		stop=1;

		for(i=6; i>=0; i--)
		{
			free_faces_bis[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces_bis[i]==NULL)
			{
				fprintf(stderr, "cca_collapse: Error, could not create new list (2).\n");
				return(-1);
			}
		}

		for(i=6; i>=0; i--)
		{
			face=(int)pow(2.0, (double)i);

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					cca_collapse_face(cca_image, l, rs, ps, face, t);

					//And look if the elements contained in the face are also free
					cca_check_free_faces_after_collapse(cca_image, inhibit, face, t, free_faces_bis, l, rs, ps);

					stop=0;
				}
			}
		}

		for(i=0; i<7; i++)
		{
			//On détruit la liste
			list_delete(free_faces[i], NO_FREE_DATA);
			//Et on permute
			free_faces[i]=free_faces_bis[i];
			free_faces_bis[i]=NULL;
		}

		count++;
	}

	/**********************************
	Third, delete the remaining structures...
	***********************************/

	for(i=0; i<7; i++)
		list_delete(free_faces[i], NO_FREE_DATA);


	/*******************************
	Debug, check for free faces
	*******************************/
	if(numloop==0)
	{
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<(int32_t)rs; i++)
				{
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_PT)))
						fprintf(stdout, "Warning, still free face CC_PT %d\n", l);

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ)))
						fprintf(stdout, "Warning, still free face CC_AZ %d\n", l);

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY)))
						fprintf(stdout, "Warning, still free face CC_AY %d\n", l);

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX)))
						fprintf(stdout, "Warning, still free face CC_AX %d\n", l);

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
						fprintf(stdout, "Warning, still free face CC_FXY %d\n", l);

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
						fprintf(stdout, "Warning, still free face CC_FXZ %d\n", l);

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
						fprintf(stdout, "Warning, still free face CC_FYZ %d\n", l);

					l++;
				}
	}

	return(0);
}


int32_t cca_add_free_face_to_list(list **free_faces, unsigned char facetype, unsigned char dir, uint32_t pixnum)
{
	switch(facetype)
	{
		case(CC_PT):
			switch(dir)
			{
				case(CC_VERT_COLL_XUP):
					list_push_uint32_t(free_faces[0], pixnum);
					break;

				case(CC_VERT_COLL_XDOWN):
					list_push_uint32_t(free_faces[1], pixnum);
					break;

				case(CC_VERT_COLL_YUP):
					list_push_uint32_t(free_faces[2], pixnum);
					break;

				case(CC_VERT_COLL_YDOWN):
					list_push_uint32_t(free_faces[3], pixnum);
					break;

				case(CC_VERT_COLL_ZUP):
					list_push_uint32_t(free_faces[4], pixnum);
					break;

				case(CC_VERT_COLL_ZDOWN):
					list_push_uint32_t(free_faces[5], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_PT.\n");
					return(-1);
			}
			break;

		case(CC_AX):
			switch(dir)
			{
				case(CC_EDGE_COLL_DIR1): //CC_FXY UP
					list_push_uint32_t(free_faces[6], pixnum);
					break;

				case(CC_EDGE_COLL_DIR2): //CC_FXY DOWN
					list_push_uint32_t(free_faces[7], pixnum);
					break;

				case(CC_EDGE_COLL_DIR3): //CC_FXZ UP
					list_push_uint32_t(free_faces[8], pixnum);
					break;

				case(CC_EDGE_COLL_DIR4): //CC_FXZ DOWN
					list_push_uint32_t(free_faces[9], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_AX.\n");
					return(-1);
			}
			break;

		case(CC_AY):
			switch(dir)
			{
				case(CC_EDGE_COLL_DIR1): //CC_FXY UP
					list_push_uint32_t(free_faces[10], pixnum);
					break;

				case(CC_EDGE_COLL_DIR2): //CC_FXY DOWN
					list_push_uint32_t(free_faces[11], pixnum);
					break;

				case(CC_EDGE_COLL_DIR3): //CC_FYZ UP
					list_push_uint32_t(free_faces[12], pixnum);
					break;

				case(CC_EDGE_COLL_DIR4): //CC_FYZ DOWN
					list_push_uint32_t(free_faces[13], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_AY.\n");
					return(-1);
			}
			break;

		case(CC_AZ):
			switch(dir)
			{
				case(CC_EDGE_COLL_DIR1): //CC_FXZ UP
					list_push_uint32_t(free_faces[14], pixnum);
					break;

				case(CC_EDGE_COLL_DIR2): //CC_FXZ DOWN
					list_push_uint32_t(free_faces[15], pixnum);
					break;

				case(CC_EDGE_COLL_DIR3): //CC_FYZ UP
					list_push_uint32_t(free_faces[16], pixnum);
					break;

				case(CC_EDGE_COLL_DIR4): //CC_FYZ DOWN
					list_push_uint32_t(free_faces[17], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_AZ.\n");
					return(-1);
			}
			break;

		case(CC_FXY):
			switch(dir)
			{
				case(CC_FACE_COLL_UP):
					list_push_uint32_t(free_faces[18], pixnum);
					break;

				case(CC_FACE_COLL_DOWN):
					list_push_uint32_t(free_faces[19], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_FXY.\n");
					return(-1);
			}
			break;

		case(CC_FXZ):
			switch(dir)
			{
				case(CC_FACE_COLL_UP):
					list_push_uint32_t(free_faces[20], pixnum);
					break;

				case(CC_FACE_COLL_DOWN):
					list_push_uint32_t(free_faces[21], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_FXZ.\n");
					return(-1);
			}
			break;

		case(CC_FYZ):
			switch(dir)
			{
				case(CC_FACE_COLL_UP):
					list_push_uint32_t(free_faces[22], pixnum);
					break;

				case(CC_FACE_COLL_DOWN):
					list_push_uint32_t(free_faces[23], pixnum);
					break;

				default:
					fprintf(stderr, "cca_add_free_face_to_list(): Error, wrong direction for CC_FYZ.\n");
					return(-1);
			}
			break;

		default:
			fprintf(stderr, "cca_add_free_face_to_list(): Wrong face type\n");
			return(-1);
	}

	return(0);
}


unsigned char cca_get_face_type_from_list_index(uint32_t index)
{
	switch(index)
	{
		case(0):
		case(1):
		case(2):
		case(3):
		case(4):
		case(5):
			return(CC_PT);
			break;

		case(6):
		case(7):
		case(8):
		case(9):
			return(CC_AX);
			break;

		case(10):
		case(11):
		case(12):
		case(13):
			return(CC_AY);
			break;

		case(14):
		case(15):
		case(16):
		case(17):
			return(CC_AZ);
			break;

		case(18):
		case(19):
			return(CC_FXY);
			break;

		case(20):
		case(21):
			return(CC_FXZ);
			break;

		case(22):
		case(23):
			return(CC_FYZ);
			break;

		default:
			fprintf(stderr, "cca_get_face_type_from_list_index(): Error, bad index given.\n");
			return(EMPTY);
			break;
	}
}


unsigned char cca_get_collapse_direction_from_list_index(uint32_t index)
{
	switch(index)
	{
		case(0):
			return(CC_VERT_COLL_XUP);
			break;

		case(1):
			return(CC_VERT_COLL_XDOWN);
			break;

		case(2):
			return(CC_VERT_COLL_YUP);
			break;

		case(3):
			return(CC_VERT_COLL_YDOWN);
			break;

		case(4):
			return(CC_VERT_COLL_ZUP);
			break;

		case(5):
			return(CC_VERT_COLL_ZDOWN);
			break;

		case(6):
		case(10):
		case(14):
			return(CC_EDGE_COLL_DIR1);
			break;

		case(7):
		case(11):
		case(15):
			return(CC_EDGE_COLL_DIR2);
			break;

		case(8):
		case(12):
		case(16):
			return(CC_EDGE_COLL_DIR3);
			break;

		case(9):
		case(13):
		case(17):
			return(CC_EDGE_COLL_DIR4);
			break;

		case(18):
		case(20):
		case(22):
			return(CC_FACE_COLL_UP);
			break;

		case(19):
		case(21):
		case(23):
			return(CC_FACE_COLL_DOWN);
			break;

		default:
			fprintf(stderr, "cca_get_collapse_direction_from_list_index(): Error, bad index igvzn as parameter.\n");
			return(EMPTY);
			break;
	}
}


list **cca_collapse_directional_surfacic(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, list **border_faces)
{
	cell *c;
	struct xvimage *bord;
	uint32_t N, i, j, k, l, rs, cs, d, ps, ever, m, s;
	face_desc face_cont;
	unsigned char face, stop, dir, dir2;
	static uint32_t COLLAPSE_SEQUENCE_3D_ARRAYINDEX[24] = {18, 8, 12, 4, 19, 9, 13, 5, 20, 6, 16, 2, 21, 7, 17, 3, 22, 10, 14, 0, 23, 11, 15, 1};



	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;
	N=d*ps;

	if(*border_faces == NULL)
	{
		(*border_faces)=list_newlist(MODE_FIFO, 4000);
		if((*border_faces)==NULL)
		{
			fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list (2).\n");
			return(NULL);
		}
	}


	if(free_faces==NULL)
	{
		free_faces=(list**)calloc(25, sizeof(list*)); //The last list (the 25-th) will be used as a temporary list
		//Prepare and initialize the lists which will be used...
		for(i=0; i<25; i++)
		{
			free_faces[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i]==NULL)
			{
				fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list.\n");
				return(NULL);
			}
		}


		/*****************************************
		First scan, we scan the whole image in order to
		detect the free faces...
		*******************************************/
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					for(face=CC_PT; face<CC_VOL; face=face*2)
					{
						dir=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
						if(ISFREE(face, dir))
						{
							if(cca_add_free_face_to_list(free_faces, face, dir, l) !=0)
							{
								fprintf(stdout, "cca_collapse_directional_surfacic(): Error when adding face to list.\n");
								return(NULL);
							}
						}
					}
					l++;
				}
	}


	/***********************************
	Second phase, collapse free faces
	**********************************/

	stop=0;
	ever=numloop;

	if(*border_complex==NULL)
	{
		(*border_complex)=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
		if((*border_complex)==NULL)
		{
			fprintf(stderr, "cca_collapse_directional_surfacic(): Error, memory allocation failed for image (2).\n");
			return(NULL);
		}
	}

	bord=*border_complex;




	while(stop==0 && (ever==0 || numloop>0))
	{
		stop=1;
		if(ever!=0)
			numloop--;


		if((*border_faces)->size != 0)
		{
			list_make_empty(*border_faces, NO_FREE_DATA);
		}


		//Add in bord all faces which are free
		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
			{
				face=cca_get_face_type_from_list_index(i);
				dir=cca_get_collapse_direction_from_list_index(i);

				c=list_gethead(free_faces[i]);

				while(c!=NULL)
				{
					l=list_read_uint32_t(free_faces[i], c);
					dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
					if(dir==dir2)
					{
						face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
						cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);
						list_push_uint8_t((*border_faces), face_cont.face_type);
						list_push_uint32_t((*border_faces), face_cont.pixnumber);
					}
					c=list_gonext(c);
				}
			}
		}




		for(i=0; i<24; i++)
		{
			j=COLLAPSE_SEQUENCE_3D_ARRAYINDEX[i];
			face=cca_get_face_type_from_list_index(j);
			dir=cca_get_collapse_direction_from_list_index(j);

			s=list_getsize(free_faces[j]);


			for(m=0; m<s; m++)
			{
				l=list_pop_uint32_t(free_faces[j]);
				dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);

				if(dir==dir2)
				{
					//Check if the face is part of the border
					face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
					if(	((UCHARDATA(bord)[face_cont.pixnumber] & face_cont.face_type)!=0) )
					{
						cca_collapse_face(cca_image, l, rs, ps, face, dir);
						stop=0;
						list_push_uint32_t(free_faces[24], l);
					}
					else
						cca_add_free_face_to_list(free_faces, face, dir, l);
				}
			}

			s=list_getsize(free_faces[24]);
			c=list_gethead(free_faces[24]);
			for(m=0; m<s; m++)
			{
				cca_check_free_faces_after_collapse_dirstrat(cca_image, inhibit, face, dir, free_faces, free_faces, list_read_uint32_t(free_faces[24], c), rs, ps);
				c=list_gonext(c);
			}

			list_make_empty(free_faces[24], NO_FREE_DATA);
		}

		c=list_gethead(*border_faces);
		while(c!=NULL)
		{
			c=list_gonext(c);
			l=list_read_uint32_t(*border_faces, c);
			cca_remove_complexface(bord, CC_VOL, l, rs, ps);
			c=list_gonext(c);
		}

		/*c=list_gethead(*border_faces);
		while(c!=NULL)
		{
			l=list_read_uint32_t(*border_faces, c);
			cca_remove_complexface(bord, CC_VOL, l, rs, ps);
			c=list_gonext(c);
		}*/

		/*for(i=0; i<N; i++)
			if(UCHARDATA(bord)[i]!=0) fprintf(stdout, "aie\n");*/
	}


	return(free_faces);
}


list **cca_collapse_directional_surfacic_for_pgm_purpose(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, struct xvimage *cube_survive, uint32_t *cpt_cube_survive)
{
	list *current_free; //One list per face type and per direction...
	cell *c;
	struct xvimage *bord, *tbord;
	uint32_t N, i, j, k, l, rs, cs, d, ps, ever, x, y, z, rs_pgm, cs_pgm, N_pgm, ps_pgm, d_pgm;
	face_desc face_cont;
	unsigned char face, stop, dir, dir2;
	static uint32_t COLLAPSE_SEQUENCE_3D_ARRAYINDEX[24] = {18, 8, 12, 4, 19, 9, 13, 5, 20, 6, 16, 2, 21, 7, 17, 3, 22, 10, 14, 0, 23, 11, 15, 1};
	char mode=2;


	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;
	N=d*ps;

	if(d>2)
		mode=3;

	rs_pgm=rs-1;
	cs_pgm=cs-1;
	d_pgm=d-1;
	ps_pgm=cs_pgm*rs_pgm;
	N_pgm=ps_pgm*d_pgm;



	//Prepare and initialize the lists which will be used...
	current_free=list_newlist(MODE_FIFO, 1024);
	if(current_free==NULL)
	{
		fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list (3).\n");
		return(NULL);
	}


	if(free_faces==NULL)
	{
		free_faces=(list**)calloc(24, sizeof(list*));
		//Prepare and initialize the lists which will be used...
		for(i=0; i<24; i++)
		{
			free_faces[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i]==NULL)
			{
				fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list.\n");
				return(NULL);
			}
		}


		/*****************************************
		First scan, we scan the whole image in order to
		detect the free faces...
		*******************************************/
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					for(face=CC_PT; face<CC_VOL; face=face*2)
					{
						dir=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
						if(ISFREE(face, dir))
						{
							if(cca_add_free_face_to_list(free_faces, face, dir, l) !=0)
							{
								fprintf(stdout, "cca_collapse_directional_surfacic(): Error when adding face to list.\n");
								return(NULL);
							}
						}
					}
					l++;
				}
	}


	/***********************************
	Second phase, collapse free faces
	**********************************/

	stop=0;
	ever=numloop;

	if(*border_complex==NULL)
	{
		(*border_complex)=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
		if((*border_complex)==NULL)
		{
			fprintf(stderr, "cca_collapse_directional_surfacic(): Error, memory allocation failed for image (2).\n");
			return(NULL);
		}
	}

	bord=*border_complex;




	while(stop==0 && (ever==0 || numloop>0))
	{
		stop=1;
		if(ever!=0)
			numloop--;



		//Add in bord all faces which are free
		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
			{
				face=cca_get_face_type_from_list_index(i);
				dir=cca_get_collapse_direction_from_list_index(i);

				c=list_gethead(free_faces[i]);
				l=list_read_uint32_t(free_faces[i], c);
				dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
				if(ISFREE(face, dir2) && dir==dir2)
					cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);

				while(list_hasnext(c))
				{
					c=list_gonext(c);
					l=list_read_uint32_t(free_faces[i], c);
					dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
					face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
					if(ISFREE(face, dir2) && dir==dir2)
						cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);
				}
			}
		}


		for(i=0; i<24; i++)
		{
			j=COLLAPSE_SEQUENCE_3D_ARRAYINDEX[i];
			face=cca_get_face_type_from_list_index(j);
			dir=cca_get_collapse_direction_from_list_index(j);

			list_append_to_other_list(free_faces[j], current_free);

			while(!list_isempty(current_free))
			{
				l=list_pop_uint32_t(current_free);
				dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);

				if(ISFREE(face, dir2) && dir==dir2)
				{
					//Check if the face is part of the border
					face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
					if(	((UCHARDATA(bord)[l] & face) !=0) &&
						((UCHARDATA(bord)[face_cont.pixnumber] & face_cont.face_type)!=0) )
					{
						if( (mode==2 && face_cont.face_type==CC_FXY) || (mode==3 && face_cont.face_type==CC_VOL) )
						{
							x=getxfrompixnum(face_cont.pixnumber, rs, ps);
							y=getyfrompixnum(face_cont.pixnumber, rs, ps);
							z=getzfrompixnum(face_cont.pixnumber, rs, ps);
							ULONGDATA(cube_survive)[z*ps_pgm+y*rs_pgm+x]=*cpt_cube_survive;
							*cpt_cube_survive=(*cpt_cube_survive)+1;
						}
						cca_collapse_face(cca_image, l, rs, ps, face, dir);
						stop=0;
						cca_check_free_faces_after_collapse_dirstrat(cca_image, inhibit, face, dir, free_faces, free_faces, l, rs, ps);
					}
					else
						cca_add_free_face_to_list(free_faces, face, dir, l);
				}
			}
		}

		memset(bord->image_data, 0, N*sizeof(unsigned char));
	}

	list_delete(current_free, NO_FREE_DATA);

	return(free_faces);
}


//The border_complex and temp_border_complex images must not have been allocated before the function starts, and must be freed by the user at the end
list **cca_collapse_directional_surfacic_enhanced(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop, list **free_faces, struct xvimage **border_complex, struct xvimage **temp_border_complex)
{
	list *current_free; //One list per face type and per direction...
	cell *c;
	struct xvimage *bord, *tbord, *ttbord;
	uint32_t N, i, j, k, l, rs, cs, d, ps, ever, e;
	face_desc face_cont;
	unsigned char face, stop, dir, dir2;
	static uint32_t COLLAPSE_SEQUENCE_3D_ARRAYINDEX[24] = {18, 8, 12, 4, 19, 9, 13, 5, 20, 6, 16, 2, 21, 7, 17, 3, 22, 10, 14, 0, 23, 11, 15, 1};


	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;
	N=d*ps;


	//Prepare and initialize the lists which will be used...
	current_free=list_newlist(MODE_FIFO, 1024);
	if(current_free==NULL)
	{
		fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list (3).\n");
		return(NULL);
	}


	if(free_faces==NULL)
	{
		free_faces=(list**)calloc(24, sizeof(list*));
		//Prepare and initialize the lists which will be used...
		for(i=0; i<24; i++)
		{
			free_faces[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i]==NULL)
			{
				fprintf(stderr, "cca_collapse_directional_surfacic(): Error, could not create new list.\n");
				return(NULL);
			}
		}


		/*****************************************
		First scan, we scan the whole image in order to
		detect the free faces...
		*******************************************/
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					for(face=CC_PT; face<CC_VOL; face=face*2)
					{
						dir=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
						if(ISFREE(face, dir))
						{
							if(cca_add_free_face_to_list(free_faces, face, dir, l) !=0)
							{
								fprintf(stdout, "cca_collapse_directional_surfacic(): Error when adding face to list.\n");
								return(NULL);
							}
						}
					}
					l++;
				}
	}


	/***********************************
	Second phase, collapse free faces
	**********************************/

	stop=0;
	ever=numloop;

	if(*border_complex==NULL)
	{
		(*border_complex)=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
		if((*border_complex)==NULL)
		{
			fprintf(stderr, "cca_collapse_directional_surfacic(): Error, memory allocation failed for image (2).\n");
			return(NULL);
		}
	}

	if(*temp_border_complex==NULL)
	{
		(*temp_border_complex)=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
		if((*temp_border_complex)==NULL)
		{
			fprintf(stderr, "cca_collapse_directional_surfacic(): Error, memory allocation failed for image (3).\n");
			return(NULL);
		}
	}

	bord=*border_complex;
	tbord=*temp_border_complex;






	while(stop==0 && (ever==0 || numloop>0))
	{
		stop=1;
		if(ever!=0)
			numloop--;

		//Add in bord all faces which are free
		for(i=0; i<24; i++)
		{
			if(!list_isempty(free_faces[i]))
			{
				face=cca_get_face_type_from_list_index(i);
				dir=cca_get_collapse_direction_from_list_index(i);

				c=list_gethead(free_faces[i]);
				l=list_read_uint32_t(free_faces[i], c);
				dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
				if(ISFREE(face, dir2) && dir==dir2)
					cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);

				while(list_hasnext(c))
				{
					c=list_gonext(c);
					l=list_read_uint32_t(free_faces[i], c);
					dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
					face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
					if(ISFREE(face, dir2) && dir==dir2)
						cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);
				}
			}
		}

		for(e=0; e<3; e++)
		{
			for(i=0; i<24; i++)
			{
				j=COLLAPSE_SEQUENCE_3D_ARRAYINDEX[i];
				face=cca_get_face_type_from_list_index(j);
				dir=cca_get_collapse_direction_from_list_index(j);

				list_append_to_other_list(free_faces[j], current_free);

				while(!list_isempty(current_free))
				{
					l=list_pop_uint32_t(current_free);
					dir2=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);

					if(ISFREE(face, dir2) && dir==dir2)
					{
						//Check if the face is part of the border
						face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
						if(	((UCHARDATA(bord)[l] & face) !=0) &&
							((UCHARDATA(bord)[face_cont.pixnumber] & face_cont.face_type)!=0) )
						{
							cca_collapse_face(cca_image, l, rs, ps, face, dir);
							cca_check_free_faces_after_collapse_dirstrat(cca_image, inhibit, face, dir, free_faces, free_faces, l, rs, ps);
						}
						else
							cca_add_free_face_to_list(free_faces, face, dir, l);
					}
				}
			}

			ttbord=bord;
			bord=tbord;
			tbord=ttbord;
			memset(bord->image_data, 0, N*sizeof(unsigned char));


			if(e!=2)
			{
				//Add in bord all faces which were previously in the border and which are now free
				for(i=0; i<24; i++)
				{
					if(!list_isempty(free_faces[i]))
					{
						face=cca_get_face_type_from_list_index(i);
						dir=cca_get_collapse_direction_from_list_index(i);

						c=list_gethead(free_faces[i]);
						l=list_read_uint32_t(free_faces[i], c);
						face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
						if( (UCHARDATA(tbord)[face_cont.pixnumber] & face_cont.face_type) !=0)
							cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);

						while(list_hasnext(c))
						{
							c=list_gonext(c);
							l=list_read_uint32_t(free_faces[i], c);
							face_cont=cca_get_container_of_free_face(face, l, dir, rs, ps);
							if( (UCHARDATA(tbord)[face_cont.pixnumber] & face_cont.face_type) !=0)
								cca_add_complexface(bord, face_cont.face_type, face_cont.pixnumber, rs, ps);
						}
					}
				}
			}
		}
	}

	list_delete(current_free, NO_FREE_DATA);

	return(free_faces);
}


int32_t cca_collapse_method_facesandsubfaces2(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop)
{
	list *free_faces[7]; //One list per face type...
	list *free_faces_bis[7], *empty_list[7];
	uint32_t N, j, k, l, rs, cs, d, ps, t, count, container_pix, p, v;
	int32_t i, did_collapse;
	unsigned char face, stop, container_type;
	struct xvimage *previous, *new_inhibit;

	count=0;

	//Prepare and initialize the lists which will be used...
	for(i=0; i<7; i++)
	{
		free_faces[i] = list_newlist(MODE_FIFO, 1024);
		if(free_faces[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Error, could not create new list.\n");
			return(-1);
		}

		empty_list[i] = list_newlist(MODE_FIFO,1024); //This list must be empty, and used for a function (avoid reallocation at each call)
		if(empty_list[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Error, could not create new list (2).\n");
			return(-1);
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;

	container_pix=0;
	container_type=0;

	previous=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
	did_collapse=0;


	/*****************************************
	First scan, we scan the whole image in order to
	detect the free faces...
	*******************************************/
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<(int32_t)rs; i++)
			{
				if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_PT)))
					list_push_uint32_t(free_faces[0], l);

				if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ)))
					list_push_uint32_t(free_faces[1], l);

				if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY)))
					list_push_uint32_t(free_faces[2], l);

				if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX)))
					list_push_uint32_t(free_faces[3], l);

				if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
				{
					list_push_uint32_t(free_faces[4], l);
					UCHARDATA(previous)[l]|=CC_FXY;
				}

				if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
				{
					list_push_uint32_t(free_faces[5], l);
					UCHARDATA(previous)[l]|=CC_FXZ;
				}

				if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
				{
					list_push_uint32_t(free_faces[6], l);
					UCHARDATA(previous)[l]|=CC_FYZ;
				}

				l++;
			}



	/***********************************
	Second phase, collapse free faces
	**********************************/
	//previous=copyimage(cca_image);

	stop=0;
	while(stop==0 && (numloop==0 || count<numloop))
	{
		v=0;
		previous=copyimage(cca_image);

		stop=1;

		for(i=0; i<7; i++)
		{
			free_faces_bis[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces_bis[i]==NULL)
			{
				fprintf(stderr, "cca_collapse: Error, could not create new list (2).\n");
				return(-1);
			}
		}

		//First, collapse of faces into volumes...
		face=CC_VOL;
		for(i=6; i>=4; i--)
		{
			face=face/2;

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				container_type=CC_VOL;
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					switch(face)
					{
						case(CC_FXY):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-ps;
							break;

						case(CC_FXZ):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-rs;
							break;

						case(CC_FYZ):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-1;
							break;

						default:
							fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type.\n");
							return(-1);
					}


					if(cca_collapse_face_inside_container(previous, inhibit, l, rs, ps, face, t, container_pix, container_type, free_faces_bis, empty_list)<0)
					{
						fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Collapse function did not work.\n");
						return(-1);
					}

					did_collapse=1;


					//Copy results of collapse into the pixel of cca_image
					UCHARDATA(cca_image)[container_pix]&=UCHARDATA(previous)[container_pix];
					UCHARDATA(cca_image)[container_pix+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_AX) | (UCHARDATA(previous)[container_pix+1] & (CC_FYZ | CC_AY | CC_AZ | CC_PT));
					UCHARDATA(cca_image)[container_pix+rs] &= (CC_VOL | CC_FXY | CC_FYZ | CC_AY) | (UCHARDATA(previous)[container_pix+rs] & (CC_FXZ | CC_AX | CC_AZ | CC_PT));
					UCHARDATA(cca_image)[container_pix+ps] &= (CC_VOL | CC_FYZ | CC_FXZ | CC_AZ) | (UCHARDATA(previous)[container_pix+ps] & (CC_FXY | CC_AY | CC_AX | CC_PT));
					UCHARDATA(cca_image)[container_pix+ps+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+rs] & (CC_AX | CC_PT));
					UCHARDATA(cca_image)[container_pix+ps+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AX | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+1] & (CC_AY | CC_PT));
					UCHARDATA(cca_image)[container_pix+1+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AX) | (UCHARDATA(previous)[container_pix+1+rs] & (CC_AZ | CC_PT));
					UCHARDATA(cca_image)[container_pix+ps+rs+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+ps+rs+1] & (CC_PT));

					cca_add_complexface(previous, container_type, container_pix, rs, ps);

					stop=0;
				}

			}
		}


		while(!list_isempty(free_faces_bis[3]))
		{
			l=list_pop_uint32_t(free_faces_bis[3]);
			list_push_uint32_t(free_faces[3], l);
		}
		while(!list_isempty(free_faces_bis[2]))
		{
			l=list_pop_uint32_t(free_faces_bis[2]);
			list_push_uint32_t(free_faces[2], l);
		}
		while(!list_isempty(free_faces_bis[1]))
		{
			l=list_pop_uint32_t(free_faces_bis[1]);
			list_push_uint32_t(free_faces[1], l);
		}


		freeimage(previous);
		previous=copyimage(cca_image);

		new_inhibit=copyimage(inhibit);
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(p=0; p<rs; p++)
				{
					if(cca_cardinal_containers(cca_image, l, p, j, k, CC_AX, rs, ps)>2)
						UCHARDATA(new_inhibit)[l] |= CC_AX;

					if(cca_cardinal_containers(cca_image, l, p, j, k, CC_AY, rs, ps)>2)
						UCHARDATA(new_inhibit)[l] |= CC_AY;

					if(cca_cardinal_containers(cca_image, l, p, j, k, CC_AZ, rs, ps)>2)
						UCHARDATA(new_inhibit)[l] |= CC_AZ;

					l++;
				}

		writeimage(cca_image, (char*)"before2.cc");

		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(p=0; p<rs; p++)
				{
					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, new_inhibit, l, rs, ps, CC_AZ)))
						list_push_uint32_t(free_faces[1], l);

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, new_inhibit, l, rs, ps, CC_AY)))
						list_push_uint32_t(free_faces[2], l);

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, new_inhibit, l, rs, ps, CC_AX)))
						list_push_uint32_t(free_faces[3], l);

					l++;


				}


		//Now, we have to collapse the faces... It will be a slightly different algorithm
		for(i=3; i>=1; i--)
		{
			face=face/2;

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, new_inhibit, l, rs, ps, face);
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					switch(face)
					{
						case(CC_AX):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXY;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXY;
								container_pix=l-rs;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FXZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FXZ;
								container_pix=l-ps;
							}
							break;

						case(CC_AY):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXY;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXY;
								container_pix=l-1;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FYZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FYZ;
								container_pix=l-ps;
							}
							break;

						case(CC_AZ):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXZ;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXZ;
								container_pix=l-1;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FYZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FYZ;
								container_pix=l-rs;
							}
							break;
					}


					//First, save the local configuration around the container pixels
					//x=container_pix%rs;
					//z=container_pix/ps;
					//y=(container_pix%ps)/rs;

					/*if(x>=(rs-1) || y>=(cs-1) || z>=(d-1))
					{
						if(container_pix+rs+ps+1 >= N)
							fprintf(stdout, "Alert\n");
					}*/

					//bxmin=x;
					//bzmin=z;
					//bymin=y;

					/*bxmax=rs-1;
					if(x<rs-1)
						bxmax=x+1;

					bymax=cs-1;
					if(y<cs-1)
						bymax=y+1;

					bzmax=d-1;
					if(z<d-1)
						bzmax=z+1;

					switch(container_type)
					{
						case(CC_FXY):
							if(z>0)
								bzmin=z-1;
							break;

						case(CC_FXZ):
							if(y>0)
								bymin=y-1;
							break;

						case(CC_FYZ):
							if(x>0)
								bxmin=x-1;
							break;

						default:
							fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type (2).\n");
							return(-1);
					}

					for(z=bzmin; z<=bzmax; z++)
						for(y=bymin; y<=bymax; y++)
							for(x=bxmin; x<=bxmax; x++)
							{
								savetab[x-bxmin][y-bymin][z-bzmin]=UCHARDATA(previous)[x+y*rs+z*ps];
							}*/

					//Then, do the most possible collapse inside the cube...
					cca_collapse_face_inside_container(cca_image, new_inhibit, l, rs, ps, face, t, container_pix, container_type, free_faces_bis, empty_list);
					//cca_collapse_face_and_subfaces(previous, inhibit, l, rs, ps, face, t, free_faces_bis);
					did_collapse=1;

					/*x=container_pix%rs;
					z=container_pix/ps;
					y=(container_pix%ps)/rs;
					//Transmit the results to the cca_image
					UCHARDATA(cca_image)[container_pix] &= UCHARDATA(previous)[container_pix];
					if(x<rs-1)
						UCHARDATA(cca_image)[container_pix+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_AX) | (UCHARDATA(previous)[container_pix+1] & (CC_FYZ | CC_AY | CC_AZ | CC_PT));
					if(y<cs-1)
						UCHARDATA(cca_image)[container_pix+rs] &= (CC_VOL | CC_FXY | CC_FYZ | CC_AY) | (UCHARDATA(previous)[container_pix+rs] & (CC_FXZ | CC_AX | CC_AZ | CC_PT));
					if(z<d-1)
						UCHARDATA(cca_image)[container_pix+ps] &= (CC_VOL | CC_FYZ | CC_FXZ | CC_AZ) | (UCHARDATA(previous)[container_pix+ps] & (CC_FXY | CC_AY | CC_AX | CC_PT));
					if((z<d-1) && (y<cs-1))
						UCHARDATA(cca_image)[container_pix+ps+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+rs] & (CC_AX | CC_PT));
					if((z<d-1) && (x<rs-1))
						UCHARDATA(cca_image)[container_pix+ps+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AX | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+1] & (CC_AY | CC_PT));
					if((x<rs-1) && (y<cs-1))
						UCHARDATA(cca_image)[container_pix+1+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AX) | (UCHARDATA(previous)[container_pix+1+rs] & (CC_AZ | CC_PT));
					if((z<d-1) && (x<rs-1) && (y<cs-1))
						UCHARDATA(cca_image)[container_pix+ps+rs+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+ps+rs+1] & (CC_PT));


					stop=0;

					//If the current face is not on a border...
					if(	((container_type==CC_FXY) && (container_pix>=ps)) || ((container_type==CC_FXZ) && (container_pix>=rs)) || ((container_type==CC_FYZ) && (container_pix>=1)) )
					{
						//cca_add_complexface(previous, container_type, container_pix, rs, ps);

						switch(container_type)
						{
							case(CC_FXY):
								container_pix-=ps;
								break;

							case(CC_FXZ):
								container_pix-=rs;
								break;

							case(CC_FYZ):
								container_pix--;
								break;

							default:
								fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type (3).\n");
								return(-1);
						}

						//Collapse again, but inside the other cube containing the face.
						//Then, do the most possible collapse inside the cube...
						cca_collapse_face_and_subfaces(previous, inhibit, l, rs, ps, face, t, container_pix, CC_VOL, free_faces_bis);

						//Transmit the results to the cca_image
						x=container_pix%rs;
						z=container_pix/ps;
						y=(container_pix%ps)/rs;
						//Transmit the results to the cca_image
						UCHARDATA(cca_image)[container_pix] &= UCHARDATA(previous)[container_pix];
						if(x<rs-1)
							UCHARDATA(cca_image)[container_pix+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_AX) | (UCHARDATA(previous)[container_pix+1] & (CC_FYZ | CC_AY | CC_AZ | CC_PT));
						if(y<cs-1)
							UCHARDATA(cca_image)[container_pix+rs] &= (CC_VOL | CC_FXY | CC_FYZ | CC_AY) | (UCHARDATA(previous)[container_pix+rs] & (CC_FXZ | CC_AX | CC_AZ | CC_PT));
						if(z<d-1)
							UCHARDATA(cca_image)[container_pix+ps] &= (CC_VOL | CC_FYZ | CC_FXZ | CC_AZ) | (UCHARDATA(previous)[container_pix+ps] & (CC_FXY | CC_AY | CC_AX | CC_PT));
						if((z<d-1) && (y<cs-1))
							UCHARDATA(cca_image)[container_pix+ps+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+rs] & (CC_AX | CC_PT));
						if((z<d-1) && (x<rs-1))
							UCHARDATA(cca_image)[container_pix+ps+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AX | CC_AZ) | (UCHARDATA(previous)[container_pix+ps+1] & (CC_AY | CC_PT));
						if((x<rs-1) && (y<cs-1))
							UCHARDATA(cca_image)[container_pix+1+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AX) | (UCHARDATA(previous)[container_pix+1+rs] & (CC_AZ | CC_PT));
						if((z<d-1) && (x<rs-1) && (y<cs-1))
							UCHARDATA(cca_image)[container_pix+ps+rs+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+ps+rs+1] & (CC_PT));



					}

					//Restore previous image
					for(z=bzmin; z<=bzmax; z++)
						for(y=bymin; y<=bymax; y++)
							for(x=bxmin; x<=bxmax; x++)
							{
								UCHARDATA(previous)[x+y*rs+z*ps]=savetab[x-bxmin][y-bymin][z-bzmin];
							}*/
					//Transmit results to image


					//Restore this part in order to get back the previous system
					/*switch(container_type)
					{
						case(CC_FXY):
							UCHARDATA(cca_image)[container_pix] &= (CC_VOL | CC_FYZ | CC_FXZ | CC_AZ) | (UCHARDATA(previous)[container_pix] & (CC_FXY | CC_AX | CC_AY | CC_PT));
							UCHARDATA(cca_image)[container_pix+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AX | CC_AZ) | (UCHARDATA(previous)[container_pix+1] & (CC_AY | CC_PT));
							UCHARDATA(cca_image)[container_pix+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ) | (UCHARDATA(previous)[container_pix+rs] & (CC_AX | CC_PT));
							UCHARDATA(cca_image)[container_pix+1+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+1+rs] & (CC_PT));
							break;

						case(CC_FYZ):
							UCHARDATA(cca_image)[container_pix] &= (CC_VOL | CC_FXY | CC_FXZ | CC_AX) | (UCHARDATA(previous)[container_pix] & (CC_FYZ | CC_AZ | CC_AY | CC_PT));
							UCHARDATA(cca_image)[container_pix+ps] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AX | CC_AZ) | (UCHARDATA(previous)[container_pix+ps] & (CC_AY | CC_PT));
							UCHARDATA(cca_image)[container_pix+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AX) | (UCHARDATA(previous)[container_pix+rs] & (CC_AZ | CC_PT));
							UCHARDATA(cca_image)[container_pix+ps+rs] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+ps+rs] & (CC_PT));
							break;

						case(CC_FXZ):
							UCHARDATA(cca_image)[container_pix] &= (CC_VOL | CC_FXY | CC_FYZ | CC_AY) | (UCHARDATA(previous)[container_pix] & (CC_FXZ | CC_AX | CC_AZ | CC_PT));
							UCHARDATA(cca_image)[container_pix+1] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AX) | (UCHARDATA(previous)[container_pix+1] & (CC_AZ | CC_PT));
							UCHARDATA(cca_image)[container_pix+ps] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ) | (UCHARDATA(previous)[container_pix+ps] & (CC_AX | CC_PT));
							UCHARDATA(cca_image)[container_pix+1+ps] &= (CC_VOL | CC_FXY | CC_FXZ | CC_FYZ | CC_AY | CC_AZ | CC_AX) | (UCHARDATA(previous)[container_pix+ps+1] & (CC_PT));
							break;
					}

					//Restore previous image
					cca_add_complexface(previous, container_type, container_pix, rs, ps);*/

				}
			}
		}

		while(!list_isempty(free_faces_bis[0]))
		{
			l=list_pop_uint32_t(free_faces_bis[0]);
			list_push_uint32_t(free_faces[0], l);
		}

		freeimage(previous);
		previous=copyimage(cca_image);

		freeimage(new_inhibit);
		new_inhibit=copyimage(inhibit);
		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(p=0; p<rs; p++)
				{
					if(cca_cardinal_containers(cca_image, l, p, j, k, CC_PT, rs, ps)>2)
						UCHARDATA(new_inhibit)[l] |= CC_PT;

					l++;
				}



		l=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(p=0; p<rs; p++)
				{
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, new_inhibit, l, rs, ps, CC_PT)))
						list_push_uint32_t(free_faces[0], l);
					l++;
				}

		face=face/2;

		//writeimage(cca_image, "before.cc");

		while(!list_isempty(free_faces[i]))
		{
			l=list_pop_uint32_t(free_faces[i]);

			//Is l really still a free face as we should have ?
			t=cca_is_face_free(cca_image, new_inhibit, l, rs, ps, face);
			//Yes, it is, we can do the main collapse...
			if(ISFREE(face, t))
			{
				if(t==CC_VERT_COLL_XUP)
				{
					container_type=CC_AX;
					container_pix=l;
				}
				else if(t==CC_VERT_COLL_XDOWN)
				{
					container_type=CC_AX;
					container_pix=l-1;
				}
				else if(t==CC_VERT_COLL_YUP)
				{
					container_type=CC_AY;
					container_pix=l;
				}
				else if(t==CC_VERT_COLL_YDOWN)
				{
					container_type=CC_AY;
					container_pix=l-rs;
				}
				else if(t==CC_VERT_COLL_ZUP)
				{
					container_type=CC_AZ;
					container_pix=l;
				}
				else if(t==CC_VERT_COLL_ZDOWN)
				{
					container_type=CC_AZ;
					container_pix=l-ps;
				}

				//cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, face, t, free_faces_bis);
				cca_collapse_face_inside_container(cca_image, new_inhibit, l, rs, ps, face, t, container_pix, container_type, free_faces_bis, empty_list);
				did_collapse=1;

				stop=0;
			}
		}





		for(i=0; i<7; i++)
		{
			//On détruit la liste
			list_delete(free_faces[i], NO_FREE_DATA);
			//Et on permute
			free_faces[i]=free_faces_bis[i];
			free_faces_bis[i]=NULL;
		}

		count++;

		freeimage(previous);
		freeimage(new_inhibit);

		//freeimage(previous);
		//previous=copyimage(cca_image);
	}

	/**********************************
	Third, delete the remaining structures...
	***********************************/

	for(i=0; i<7; i++)
		list_delete(free_faces[i], NO_FREE_DATA);


	return(did_collapse);
}


int32_t cca_collapse_face_and_count_intersection_edge(struct xvimage *original, struct xvimage *image_to_count_edge, struct xvimage *surfaces, uint32_t container_pix, uint32_t container_x, uint32_t container_y, uint32_t container_z, unsigned char facetype, unsigned char direction, uint32_t face_pixnumber, list **empty_lists, uint32_t rs, uint32_t ps)
{
	uint32_t tp, r;

	if(ISFREE(facetype, direction))
	{
		tp=cca_is_face_free(image_to_count_edge, NULL, face_pixnumber, rs, ps, facetype);

		if(tp==direction)
		{
			//So collapse the face
			if(cca_collapse_face_inside_container_with_two_images(original, image_to_count_edge, NULL, face_pixnumber, rs, ps, facetype, direction, container_pix, CC_VOL, NULL, empty_lists)<0)
			{
				fprintf(stderr, "cca_collapse_face_and_count_intersection_edge: Error in collapse function.\n");
				return(-1);
			}

			//Count how many edges are intersection edges
/*			r=0;
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)>=32 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)>=3 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;*/

			r=0;
			if (cca_cardinal_containers(surfaces, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;
			if (cca_cardinal_containers(surfaces, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==1 && cca_cardinal_containers(image_to_count_edge, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==2 && cca_edge_cube_cardinal_containers(image_to_count_edge, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==0)
				r++;

			//Restore image as it was before collapse.
			cca_add_complexface(image_to_count_edge, CC_VOL, container_pix, rs, ps);
			cca_add_complexface(original, CC_VOL, container_pix, rs, ps);

			//End
			return(r);
		}
		else
		{
			return(-1);
		}
	}

	return(-1);
}


int32_t cca_collapse_face_and_count_isolated_faces(struct xvimage *original, struct xvimage *image_to_count_edge, struct xvimage *inhibit, uint32_t container_pix, uint32_t container_x, uint32_t container_y, uint32_t container_z, unsigned char facetype, unsigned char direction, uint32_t face_pixnumber, list **empty_lists, uint32_t rs, uint32_t ps)
{
	uint32_t tp, code_face, code_surf_edges, cx, cy, cz;

	if(ISFREE(facetype, direction))
	{
		tp=cca_is_face_free(image_to_count_edge, NULL, face_pixnumber, rs, ps, facetype);

		if(tp==direction)
		{
			//So collapse the face
			if(cca_collapse_face_inside_container_with_two_images(original, image_to_count_edge, inhibit, face_pixnumber, rs, ps, facetype, direction, container_pix, CC_VOL, NULL, empty_lists)<0)
			{
				fprintf(stderr, "cca_collapse_face_and_count_intersection_edge: Error in collapse function.\n");
				return(-1);
			}

			code_face=0;
			code_surf_edges=0;
			//Is this face still in a cube ?

			/*We code face in the cube as following

				  _______________
				 /               /|
				/       0       / |
			   /______________ /  |
			   |          (1)  |  |
			   |(2)            |5 |
			   |      4        |  /
			   |      (3)      | /
			   |_______________|/
			   */

			if( (UCHARDATA(image_to_count_edge)[container_pix]&CC_FXY) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_FXY, rs, ps)==0)
				code_face|=1;
			if( (UCHARDATA(image_to_count_edge)[container_pix]&CC_FXZ) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_FXZ, rs, ps)==0)
				code_face|=2;
			if( (UCHARDATA(image_to_count_edge)[container_pix]&CC_FYZ) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_FYZ, rs, ps)==0)
				code_face|=4;
			if( (UCHARDATA(image_to_count_edge)[container_pix+ps]&CC_FXY) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z+1, CC_FXY, rs, ps)==0)
				code_face|=8;
			if( (UCHARDATA(image_to_count_edge)[container_pix+rs]&CC_FXZ) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y+1, container_z, CC_FXZ, rs, ps)==0)
				code_face|=16;
			if( (UCHARDATA(image_to_count_edge)[container_pix+1]&CC_FYZ) != 0 && cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x+1, container_y, container_z, CC_FYZ, rs, ps)==0)
				code_face|=32;

			//Is this edge an intersection edge, or a surface edge

			/*We code edges in a cube as following (the index is the place of the bit)


                   _______0_______
                  /              /|
				3              1  |
			   /_______2______ /  4
			   |   7           |  |
			   |               |  |
			   6        8      5  /
			   | 11            | 9
			   |____10_________|/
*/
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AX, rs, ps)==2)
				code_surf_edges|=1;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AX, rs, ps)==2)
				code_surf_edges|=4;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AX, rs, ps)==2)
				code_surf_edges|=256;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs+ps, container_x, container_y, container_z, CC_AX, rs, ps)==2)
				code_surf_edges|=1024;
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AY, rs, ps)==2)
				code_surf_edges|=8;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AY, rs, ps)==2)
				code_surf_edges|=2;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+ps, container_x, container_y, container_z, CC_AY, rs, ps)==2)
				code_surf_edges|=2048;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1+ps, container_x, container_y, container_z, CC_AY, rs, ps)==2)
				code_surf_edges|=512;
			if (cca_cardinal_containers(image_to_count_edge, container_pix, container_x, container_y, container_z, CC_AZ, rs, ps)==2)
				code_surf_edges|=128;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1, container_x, container_y, container_z, CC_AZ, rs, ps)==2)
				code_surf_edges|=16;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==2)
				code_surf_edges|=64;
			if (cca_cardinal_containers(image_to_count_edge, container_pix+1+rs, container_x, container_y, container_z, CC_AZ, rs, ps)==2)
				code_surf_edges|=32;


			cx=346009%rs;
			cy=(346009%ps)/rs;
			cz=346009/ps;

			/*if( cca_cardinal_containers(image_to_count_edge, 346009, cx, cy, cz, CC_FXZ, rs, ps)==0 &&
				cca_cardinal_containers(image_to_count_edge, 346009, cx, cy, cz, CC_AX, rs, ps)>2 &&
				cca_cardinal_containers(image_to_count_edge, 346009, cx, cy, cz, CC_AZ, rs, ps)>2 &&
				cca_cardinal_containers(image_to_count_edge, 346009+1, cx+1, cy, cz, CC_AZ, rs, ps)>2 &&
				cca_cardinal_containers(image_to_count_edge, 346009+ps, cx, cy, cz+1, CC_AX, rs, ps)>2)
				{
					fprintf(stdout, "beurk\n");
					return(-1);
				}*/

			//Restore image as it was before collapse.
			cca_add_complexface(image_to_count_edge, CC_VOL, container_pix, rs, ps);
			cca_add_complexface(original, CC_VOL, container_pix, rs, ps);

			//End
			return(count_isolated_faces(code_face, code_surf_edges));
		}
		else
		{
			return(-1);
		}
	}

	return(-1);
}


int32_t cca_collapse_method_facesandsubfaces_avoid_intersections(struct xvimage *cca_image, struct xvimage *inhibit)
{
	list *free_faces[7], *free_faces_bis[7], *empty_list[7], *temp_list[7], *temp; //One list per face type...
	uint32_t N, j, k, l, rs, cs, d, ps, t, cx, cy, cz, container_pix, v, c;
	int32_t i, debugi;
	unsigned char face, stop, container_type;

	debugi=0;
	//Prepare and initialize the lists which will be used...
	for(i=0; i<7; i++)
	{
		free_faces[i] = list_newlist(MODE_FIFO, 1024); //This list contains free faces of turn n
		if(free_faces[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error, could not create new list.\n");
			return(-1);
		}

		free_faces_bis[i] = list_newlist(MODE_FIFO, 1024); //This list contains free faces of turn n+1
		if(free_faces_bis[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error, could not create new list (2).\n");
			return(-1);
		}

		empty_list[i] = list_newlist(MODE_FIFO, 1024); //This list must be empty, and used for a function (avoid reallocation at each call)
		if(empty_list[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error, could not create new list (3).\n");
			return(-1);
		}

		temp_list[i] = list_newlist(MODE_FIFO, 1024); //This list contains free faces after collapse of face... If finnaly, it turns out the face must not be collapsed, we deleted this list, else we copy it into free_faces_bis
		if(temp_list[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error, could not create new list (4).\n");
			return(-1);
		}
	}

	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;
	N=d*ps;

/*	container_pix=0;
	for(cz=0; cz<d; cz++)
		for(cy=0; cy<cs; cy++)
			for(cx=0; cx<rs; cx++)
			{
				if(cca_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AX, rs, ps)>=3 && cca_edge_cube_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AX, rs, ps)==0)
				{
					fprintf(stdout, "Aie.\n");
				}
				container_pix++;
			}
*/

	//First scan, we scan the whole image in order to
	//detect the free faces...
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<(int32_t)rs; i++)
			{
				if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_PT)))
					list_push_uint32_t(free_faces[0], l);

				if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ)))
					list_push_uint32_t(free_faces[1], l);

				if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY)))
					list_push_uint32_t(free_faces[2], l);

				if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX)))
					list_push_uint32_t(free_faces[3], l);

				if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
					list_push_uint32_t(free_faces[4], l);

				if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
					list_push_uint32_t(free_faces[5], l);

				if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
					list_push_uint32_t(free_faces[6], l);

				l++;
			}

	//Second phase, collapse free faces
	stop=0;
	c=0;
	while(stop==0)
	{
		v=0;
		stop=1;

		//First, collapse of faces into volumes...
		//Don't collapse immediatly the faces if they give birth to "unwanted" edges
		face=CC_VOL;
		for(i=6; i>=4; i--)
		{
			face=face/2;

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				container_type=CC_VOL;
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					switch(face)
					{
						case(CC_FXY):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-ps;
							break;

						case(CC_FXZ):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-rs;
							break;

						case(CC_FYZ):
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-1;
							break;

						default:
							fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Wrong face_type.\n");
							return(-1);
					}

					if(cca_collapse_face_inside_container(cca_image, inhibit, l, rs, ps, face, t, container_pix, container_type, temp_list, empty_list)<0)
					{
						fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error in collapse function.\n");
						return(-1);
					}


					cx=container_pix%rs;
					cy=(container_pix%ps)/rs;
					cz=container_pix/ps;
					if(		((UCHARDATA(cca_image)[container_pix]&CC_AX)!=0 && cca_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AX, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AX, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+rs]&CC_AX)!=0 && cca_cardinal_containers(cca_image, container_pix+rs, cx, cy, cz, CC_AX, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+rs, cx, cy, cz, CC_AX, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+ps]&CC_AX)!=0 && cca_cardinal_containers(cca_image, container_pix+ps, cx, cy, cz, CC_AX, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+ps, cx, cy, cz, CC_AX, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+rs+ps]&CC_AX)!=0 && cca_cardinal_containers(cca_image, container_pix+rs+ps, cx, cy, cz, CC_AX, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+rs+ps, cx, cy, cz, CC_AX, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix]&CC_AY)!=0 && cca_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AY, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AY, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+1]&CC_AY)!=0 && cca_cardinal_containers(cca_image, container_pix+1, cx, cy, cz, CC_AY, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+1, cx, cy, cz, CC_AY, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+ps]&CC_AY)!=0 && cca_cardinal_containers(cca_image, container_pix+ps, cx, cy, cz, CC_AY, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+ps, cx, cy, cz, CC_AY, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+1+ps]&CC_AY)!=0 && cca_cardinal_containers(cca_image, container_pix+1+ps, cx, cy, cz, CC_AY, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+1+ps, cx, cy, cz, CC_AY, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix]&CC_AZ)!=0 && cca_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AZ, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix, cx, cy, cz, CC_AZ, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+1]&CC_AZ)!=0 && cca_cardinal_containers(cca_image, container_pix+1, cx, cy, cz, CC_AZ, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+1, cx, cy, cz, CC_AZ, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+rs]&CC_AZ)!=0 && cca_cardinal_containers(cca_image, container_pix+rs, cx, cy, cz, CC_AZ, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+rs, cx, cy, cz, CC_AZ, rs, ps)==0)
						||	((UCHARDATA(cca_image)[container_pix+1+rs]&CC_AZ)!=0 && cca_cardinal_containers(cca_image, container_pix+1+rs, cx, cy, cz, CC_AZ, rs, ps)>2 && cca_edge_cube_cardinal_containers(cca_image, container_pix+1+rs, cx, cy, cz, CC_AZ, rs, ps)==0) )
					{
						//An unwanted edge appeared
						//Forget this face... It will come back when one of its neighbour will have been collapsed... or never...
						cca_add_complexface(cca_image, container_type, container_pix, rs, ps);
						for(j=0; j<7; j++)
							list_make_empty(temp_list[i], NO_FREE_DATA);
					}
					else
					{
						//We add the free faces of temp_list to free_faces_bis
						for(j=0; j<7; j++)
							if(list_append_to_other_list(temp_list[i], free_faces_bis[i])<0)
							{
								fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error in append list function.\n");
								return(-1);
							}

						stop=0;

						//Then, we check the neighbouring cubes that could have free faces
						//Maybe these free faces could not be collapsed because of intersections, and can now be collapsed
						if(cz>0 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1-ps, rs, ps);
						if(cz>0 && cx<rs-1)  cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1-ps, rs, ps);
						if(cz>0 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs-ps, rs, ps);
						if(cz>0 && cy<cs-1)  cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs-ps, rs, ps);
						if(cx>0)  cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1, rs, ps);
						if(cx<rs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1, rs, ps);
						if(cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs, rs, ps);
						if(cy<cs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs, rs, ps);
						if(cx>0 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1-rs, rs, ps);
						if(cx>0 && cy<cs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1+rs, rs, ps);
						if(cx<rs-1 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1-rs, rs, ps);
						if(cx<rs-1 && cy<cs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1+rs, rs, ps);
						if(cz<d-1 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1+ps, rs, ps);
						if(cz<d-1 && cx<rs-1)  cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1+ps, rs, ps);
						if(cz<d-1 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs+ps, rs, ps);
						if(cz<d-1 && cy<cs-1)  cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs+ps, rs, ps);
					}


				}

			}
		}




		//Now, we have to collapse the faces... It will be a slightly different algorithm
		for(i=3; i>=1; i--)
		{
			face=face/2;

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					switch(face)
					{
						case(CC_AX):
							switch(t)
							{
								case(CC_EDGE_COLL_DIR1):
									container_type=CC_FXY;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR2):
									container_type=CC_FXY;
									container_pix=l-rs;
									break;

								case(CC_EDGE_COLL_DIR3):
									container_type=CC_FXZ;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR4):
									container_type=CC_FXZ;
									container_pix=l-ps;
									break;

								default:
									fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong collapse direction.\n");
									return(-1);
							}
							break;

						case(CC_AY):
							switch(t)
							{
								case(CC_EDGE_COLL_DIR1):
									container_type=CC_FXY;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR2):
									container_type=CC_FXY;
									container_pix=l-1;
									break;

								case(CC_EDGE_COLL_DIR3):
									container_type=CC_FYZ;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR4):
									container_type=CC_FYZ;
									container_pix=l-ps;
									break;

								default:
									fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong collapse direction (2).\n");
									return(-1);
							}
							break;

						case(CC_AZ):
						{
							switch(t)
							{
								case(CC_EDGE_COLL_DIR1):
									container_type=CC_FXZ;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR2):
									container_type=CC_FXZ;
									container_pix=l-1;
									break;

								case(CC_EDGE_COLL_DIR3):
									container_type=CC_FYZ;
									container_pix=l;
									break;

								case(CC_EDGE_COLL_DIR4):
									container_type=CC_FYZ;
									container_pix=l-rs;
									break;

								default:
									fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong collapse direction (3).\n");
									return(-1);
							}
							break;
						}

						default:
							fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type (2).\n");
							return(-1);
					}

					if(cca_collapse_face_inside_container(cca_image, inhibit, l, rs, ps, face, t, container_pix, container_type, free_faces_bis, empty_list)<0)
					{
						fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error in collapse function (2).\n");
						return(-1);
					}

					cx=container_pix%rs;
					cy=(container_pix%ps)/rs;
					cz=container_pix/ps;

					stop=0;

					switch(container_type)
					{
						case(CC_FXY):
							if(cx>0 && cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1-ps, rs, ps);
							if(cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1, rs, ps);
							if(cx<rs-1 && cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1-ps, rs, ps);
							if(cx<rs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1, rs, ps);
							if(cy>0 && cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs-ps, rs, ps);
							if(cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs, rs, ps);
							if(cy<cs-1 && cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs-ps, rs, ps);
							if(cy<cs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs, rs, ps);
							break;

						case(CC_FXZ):
							if(cx>0 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1-rs, rs, ps);
							if(cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-1, rs, ps);
							if(cx<rs-1 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1-rs, rs, ps);
							if(cx<rs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+1, rs, ps);
							if(cz>0 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-ps-rs, rs, ps);
							if(cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-ps, rs, ps);
							if(cz<d-1 && cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+ps-rs, rs, ps);
							if(cz<d-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+ps, rs, ps);
							break;

						case(CC_FYZ):
							if(cz>0 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-ps-1, rs, ps);
							if(cz>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-ps, rs, ps);
							if(cz<d-1 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+ps-1, rs, ps);
							if(cz<d-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+ps, rs, ps);
							if(cy>0 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs-1, rs, ps);
							if(cy>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix-rs, rs, ps);
							if(cy<cs-1 && cx>0) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs-1, rs, ps);
							if(cy<cs-1) cca_check_free_faces_in_cube(cca_image, inhibit, free_faces_bis, container_pix+rs, rs, ps);
							break;
					}

				}
			}
		}


		face=face/2;

		while(!list_isempty(free_faces[i]))
		{
			l=list_pop_uint32_t(free_faces[i]);

			//Is l really still a free face as we should have ?
			t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
			//Yes, it is, we can do the main collapse...
			if(ISFREE(face, t))
			{
				switch(t)
				{
					case(CC_VERT_COLL_XUP):
						container_type=CC_AX;
						container_pix=l;
						break;

					case(CC_VERT_COLL_XDOWN):
						container_type=CC_AX;
						container_pix=l-1;
						break;

					case(CC_VERT_COLL_YUP):
						container_type=CC_AY;
						container_pix=l;
						break;

					case(CC_VERT_COLL_YDOWN):
						container_type=CC_AY;
						container_pix=l-rs;
						break;

					case(CC_VERT_COLL_ZUP):
						container_type=CC_AZ;
						container_pix=l;
						break;

					case(CC_VERT_COLL_ZDOWN):
						container_type=CC_AZ;
						container_pix=l-ps;
						break;

					default:
						fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong collapse direction (3).\n");
						return(-1);
				}

				if(cca_collapse_face_inside_container(cca_image, inhibit, l, rs, ps, face, t, container_pix, container_type, free_faces_bis, empty_list)<0)
				{
					fprintf(stderr, "cca_collapse_method_facesandsubfaces_minimize_intersections(): Error in collapse function (3).\n");
					return(-1);
				}

				stop=0;
			}
		}

		//Inverse free_faces and free_faces_bis
		for(j=0; j<7; j++)
		{
			temp=free_faces[j];
			free_faces[j]=free_faces_bis[j];
			free_faces_bis[j]=temp;
		}
	}

	//Third, delete the remaining structures...
	for(i=0; i<7; i++)
	{
		list_delete(free_faces[i], NO_FREE_DATA);
		list_delete(free_faces_bis[i], NO_FREE_DATA);
		list_delete(empty_list[i], NO_FREE_DATA);
		list_delete(temp_list[i], NO_FREE_DATA);
	}



	return(0);
}


//Method for collapsing remaining cubes and minimize the intersecting faces.
int32_t cca_collapse_cubes_avoid_create_surfaces(struct xvimage *cca_image, struct xvimage* inhibit)
{
	list *free_faces[7]; //One list per face type...
	list *empty_list[7];
	list *free_faces_bis[7];
	uint32_t N, j, k, l, rs, cs, d, m, ps, t, container_pix, v, selected_pix, cx, cy, cz;
	int32_t i, r, r_t;
	unsigned char face, stop, container_type, selected_face, selected_direction;
	struct xvimage *previous;

	//Prepare and initialize the lists which will be used...
	for(i=0; i<7; i++)
	{
		free_faces[i] = list_newlist(MODE_FIFO, 1024);
		if(free_faces[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Error, could not create new list.\n");
			return(-1);
		}

		empty_list[i] = list_newlist(MODE_FIFO, 1024); //This list must be empty, and used for a function (avoid reallocation at each call)
		if(empty_list[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Error, could not create new list (2).\n");
			return(-1);
		}

		free_faces_bis[i] = list_newlist(MODE_FIFO, 1024);
		if(free_faces_bis[i]==NULL)
		{
			fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Error, could not create new list.\n");
			return(-1);
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;

	selected_pix=0;
	selected_face=0;
	selected_direction=0;

	//previous=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);


	/*****************************************
	First scan, we scan the whole image in order to
	detect the free faces...
	*******************************************/
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<(int32_t)rs; i++)
			{
				if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
				{
					list_push_uint32_t(free_faces[4], l);
					//UCHARDATA(previous)[l]|=CC_FXY;
				}

				if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
				{
					list_push_uint32_t(free_faces[5], l);
					//UCHARDATA(previous)[l]|=CC_FXZ;
				}

				if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
				{
					list_push_uint32_t(free_faces[6], l);
					//UCHARDATA(previous)[l]|=CC_FYZ;
				}

				l++;
			}



	/***********************************
	Second phase, collapse free faces
	**********************************/

	for(m=0; m<=1; m++)
	{
		stop=0;
		while(stop==0)
		{
			v=0;
			previous=copyimage(cca_image);
			stop=1;

			//collapse of faces and volume...
			face=CC_VOL;
			for(i=6; i>=4; i--)
			{
				face=face/2;

				while(!list_isempty(free_faces[i]))
				{
					l=list_pop_uint32_t(free_faces[i]);

					//Find the volume which contains the free face.
					t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
					container_type=CC_VOL;

					if(ISFREE(face, t))
					{
						switch(face)
						{
							case(CC_FXY):
								if(t==CC_FACE_COLL_UP)
									container_pix=l;
								else
									container_pix=l-ps;
								break;

							case(CC_FXZ):
								if(t==CC_FACE_COLL_UP)
									container_pix=l;
								else
									container_pix=l-rs;
								break;

							case(CC_FYZ):
								if(t==CC_FACE_COLL_UP)
									container_pix=l;
								else
									container_pix=l-1;
								break;


							default:
								fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type.\n");
								return(-1);
						}

						if(container_pix==80520)
						{
							fprintf(stdout, "Ici.\n");
						}



						//We are going to look for the face of the volume which collapses and creates the least intersection edge possible
						r=7;
						cx=container_pix%rs;
						cy=(container_pix%ps)/rs;
						cz=container_pix/ps;

						if(cca_is_face_free(cca_image, inhibit, container_pix, rs, ps, CC_FXY)==CC_FACE_COLL_UP) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FXY, CC_FACE_COLL_UP, container_pix, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FXY;
								selected_direction=CC_FACE_COLL_UP;
								selected_pix=container_pix;
							}
						}

						if(cca_is_face_free(cca_image, inhibit, container_pix, rs, ps, CC_FXZ)==CC_FACE_COLL_UP) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FXZ, CC_FACE_COLL_UP, container_pix, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FXZ;
								selected_direction=CC_FACE_COLL_UP;
								selected_pix=container_pix;
							}
						}

						if(cca_is_face_free(cca_image, inhibit, container_pix, rs, ps, CC_FYZ)==CC_FACE_COLL_UP) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FYZ, CC_FACE_COLL_UP, container_pix, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FYZ;
								selected_direction=CC_FACE_COLL_UP;
								selected_pix=container_pix;
							}
						}

						if(cca_is_face_free(cca_image, inhibit, container_pix+ps, rs, ps, CC_FXY)==CC_FACE_COLL_DOWN) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FXY, CC_FACE_COLL_DOWN, container_pix+ps, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FXY;
								selected_direction=CC_FACE_COLL_DOWN;
								selected_pix=container_pix+ps;
							}
						}

						if(cca_is_face_free(cca_image, inhibit, container_pix+rs, rs, ps, CC_FXZ)==CC_FACE_COLL_DOWN) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FXZ, CC_FACE_COLL_DOWN, container_pix+rs, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FXZ;
								selected_direction=CC_FACE_COLL_DOWN;
								selected_pix=container_pix+rs;
							}
						}

						if(cca_is_face_free(cca_image, inhibit, container_pix+1, rs, ps, CC_FYZ)==CC_FACE_COLL_DOWN) //This face is free and collapse onto our container... Let's test it
						{
							r_t = cca_collapse_face_and_count_isolated_faces(previous, cca_image, inhibit, container_pix, cx, cy, cz, CC_FYZ, CC_FACE_COLL_DOWN, container_pix+1, empty_list, rs, ps);
							if(r_t<r)
							{
								r=r_t;
								selected_face=CC_FYZ;
								selected_direction=CC_FACE_COLL_DOWN;
								selected_pix=container_pix+1;
							}
						}


						//We have found the best collapse to do, so do it
						if((m==0 && r==0) || (m==1)) //If m=0, we want r=0 to collapse... Else, if m=1, it means this face cannot collpase without creating an isolated face, so collapse it the best way
						{
							if(cca_collapse_face_inside_container_with_two_images(previous, cca_image, inhibit, selected_pix, rs, ps, selected_face, selected_direction, container_pix, container_type, free_faces, empty_list)<0)
							{
								fprintf(stderr, "cca_collapse_method_facesandsubfaces2(): Collapse function did not work.\n");
								return(-1);
							}


							//Restore the face in the "original" image
							cca_add_complexface(previous, container_type, container_pix, rs, ps);
							stop=0;
						}
						else
						{
							list_push_uint32_t(free_faces_bis[i], l);
						}


					}

				}
			}

			list_make_empty(free_faces[3], NO_FREE_DATA);
			list_make_empty(free_faces[2], NO_FREE_DATA);
			list_make_empty(free_faces[1], NO_FREE_DATA);
			list_make_empty(free_faces[0], NO_FREE_DATA);

			list_delete(free_faces[6], NO_FREE_DATA);
			list_delete(free_faces[5], NO_FREE_DATA);
			list_delete(free_faces[4], NO_FREE_DATA);

			free_faces[6]=free_faces_bis[6];
			free_faces[4]=free_faces_bis[4];
			free_faces[5]=free_faces_bis[5];

			free_faces_bis[6]=list_newlist(MODE_FIFO, 1024);
			free_faces_bis[5]=list_newlist(MODE_FIFO, 1024);
			free_faces_bis[4]=list_newlist(MODE_FIFO, 1024);
		}
	}

	cca_collapse_method_facesandsubfaces2(cca_image, inhibit, 0);





	return(0);
}


int32_t cca_collapse_method_facesandsubfaces(struct xvimage *cca_image, struct xvimage *inhibit, uint32_t numloop)
{
	list *free_faces[7]; //On list per face type...
	list *free_faces_bis[7];
	uint32_t N, j, k, l, rs, cs, d, ps, t, count, container_pix;
	int32_t i;
	unsigned char face, stop, container_type;

	count=0;

	//Prepare and initialize the lists which will be used...
	for(i=0; i<7; i++)
	{
		free_faces[i] = list_newlist(MODE_FIFO, 1024);
		if(free_faces[i]==NULL)
		{
			fprintf(stderr, "cca_collapse(): Error, could not create new list.\n");
			return(-1);
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;


	/*****************************************
	First scan, we scan the whole image in order to
	detect the free faces...
	*******************************************/
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(t=0; t<rs; t++)
			{
				if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_PT)))
					list_push_uint32_t(free_faces[0], l);

				if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ)))
					list_push_uint32_t(free_faces[1], l);

				if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY)))
					list_push_uint32_t(free_faces[2], l);

				if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX)))
					list_push_uint32_t(free_faces[3], l);

				if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXY)))
					list_push_uint32_t(free_faces[4], l);

				if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FXZ)))
					list_push_uint32_t(free_faces[5], l);

				if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_FYZ)))
					list_push_uint32_t(free_faces[6], l);

				l++;


			}


	/***********************************
	Second phase, collapse free faces
	**********************************/
	//previous=copyimage(cca_image);

	stop=0;
	while(stop==0 && (numloop==0 || count<numloop))
	{
		stop=1;

		for(i=0; i<7; i++)
		{
			free_faces_bis[i] = list_newlist(MODE_FIFO, 1024);
			if(free_faces_bis[i]==NULL)
			{
				fprintf(stderr, "cca_collapse: Error, could not create new list (2).\n");
				return(-1);
			}
		}

		face=CC_VOL;
		for(i=6; i>=0; i--)
		{
			face=face/2;

			while(!list_isempty(free_faces[i]))
			{
				l=list_pop_uint32_t(free_faces[i]);

				//Is l really still a free face as we should have ?
				t=cca_is_face_free(cca_image, inhibit, l, rs, ps, face);
				//Yes, it is, we can do the main collapse...
				if(ISFREE(face, t))
				{
					switch(face)
					{
						case(CC_FXY):
							container_type=CC_VOL;
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-ps;
							break;

						case(CC_FXZ):
							container_type=CC_VOL;
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-rs;
							break;

						case(CC_FYZ):
							container_type=CC_VOL;
							if(t==CC_FACE_COLL_UP)
								container_pix=l;
							else
								container_pix=l-1;
							break;

						case(CC_AX):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXY;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXY;
								container_pix=l-rs;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FXZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FXZ;
								container_pix=l-ps;
							}
							break;

						case(CC_AY):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXY;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXY;
								container_pix=l-1;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FYZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FYZ;
								container_pix=l-ps;
							}
							break;

						case(CC_AZ):
							if(t==CC_EDGE_COLL_DIR1)
							{
								container_type=CC_FXZ;
								container_pix=l;
							}
							else if(t==CC_EDGE_COLL_DIR2)
							{
								container_type=CC_FXZ;
								container_pix=l-1;
							}
							else if(t==CC_EDGE_COLL_DIR3)
							{
								container_type=CC_FYZ;
								container_pix=l;
							}
							else
							{
								container_type=CC_FYZ;
								container_pix=l-rs;
							}
							break;

						case(CC_PT):
							if(t==CC_VERT_COLL_XUP)
							{
								container_type=CC_AX;
								container_pix=l;
							}
							else if(t==CC_VERT_COLL_XDOWN)
							{
								container_type=CC_AX;
								container_pix=l-1;
							}
							else if(t==CC_VERT_COLL_YUP)
							{
								container_type=CC_AY;
								container_pix=l;
							}
							else if(t==CC_VERT_COLL_YDOWN)
							{
								container_type=CC_AY;
								container_pix=l-rs;
							}
							else if(t==CC_VERT_COLL_ZUP)
							{
								container_type=CC_AZ;
								container_pix=l;
							}
							else if(t==CC_VERT_COLL_ZDOWN)
							{
								container_type=CC_AZ;
								container_pix=l-ps;
							}
							break;

						default:
							fprintf(stderr, "cca_collapse_face_and_subfaces(): Wrong face_type.\n");
							return(-1);
					}

					cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, face, t, free_faces_bis);

					stop=0;
				}
	/*			else
				{
					switch(face)
					{
						case(CC_FXY):
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX);
							if(ISFREE(CC_AX, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AX, t, l, CC_FXY, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY);
							if(ISFREE(CC_AY, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AY, t, l, CC_FXY, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+rs, rs, ps, CC_AX);
							if(ISFREE(CC_AX, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+rs, rs, ps, CC_AX, t, l, CC_FXY, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+1, rs, ps, CC_AY);
							if(ISFREE(CC_AY, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+1, rs, ps, CC_AY, t, l, CC_FXY, free_faces_bis);
							break;

						case(CC_FXZ):
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AX);
							if(ISFREE(CC_AX, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AX, t, l, CC_FXZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ);
							if(ISFREE(CC_AZ, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AZ, t, l, CC_FXZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+ps, rs, ps, CC_AX);
							if(ISFREE(CC_AX, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+ps, rs, ps, CC_AX, t, l, CC_FXZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+1, rs, ps, CC_AZ);
							if(ISFREE(CC_AZ, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+1, rs, ps, CC_AZ, t, l, CC_FXZ, free_faces_bis);
							break;

						case(CC_FYZ):
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AY);
							if(ISFREE(CC_AY, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AY, t, l, CC_FYZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l, rs, ps, CC_AZ);
							if(ISFREE(CC_AZ, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l, rs, ps, CC_AZ, t, l, CC_FYZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+ps, rs, ps, CC_AY);
							if(ISFREE(CC_AY, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+ps, rs, ps, CC_AY, t, l, CC_FYZ, free_faces_bis);
							t=cca_is_face_free(cca_image, inhibit, l+rs, rs, ps, CC_AZ);
							if(ISFREE(CC_AZ, t))
								cca_collapse_face_and_subfaces(cca_image, inhibit, l+rs, rs, ps, CC_AZ, t, l, CC_FYZ, free_faces_bis);
							break;

						default:
							break;
					}
				}*/
			}

			if(i==4)
			{
				while(!list_isempty(free_faces_bis[3]))
				{
					l=list_pop_uint32_t(free_faces_bis[3]);
					list_push_uint32_t(free_faces[3], l);
				}
				while(!list_isempty(free_faces_bis[2]))
				{
					l=list_pop_uint32_t(free_faces_bis[2]);
					list_push_uint32_t(free_faces[2], l);
				}
				while(!list_isempty(free_faces_bis[1]))
				{
					l=list_pop_uint32_t(free_faces_bis[1]);
					list_push_uint32_t(free_faces[1], l);
				}
			}

			if(i==1)
			{
				while(!list_isempty(free_faces_bis[0]))
				{
					l=list_pop_uint32_t(free_faces_bis[0]);
					list_push_uint32_t(free_faces[0], l);
				}
			}

		}

		for(i=0; i<7; i++)
		{
			//On détruit la liste
			list_delete(free_faces[i], NO_FREE_DATA);
			//Et on permute
			free_faces[i]=free_faces_bis[i];
			free_faces_bis[i]=NULL;
		}

		count++;

		//freeimage(previous);
		//previous=copyimage(cca_image);
	}

	/**********************************
	Third, delete the remaining structures...
	***********************************/

	for(i=0; i<7; i++)
		list_delete(free_faces[i], NO_FREE_DATA);


	return(0);
}

/*!	\brief Performs the surfacic collapse (preserve the main surfaces) of a cubical complex.

	\param cca_image The input image to collapse.

	\return Negative integer if error.

	\ingroup complex
	\ingroup collapse

	\author John Chaussard
*/
int32_t cca_surfacic_collapse(struct xvimage *cca_image)
{
	list *free_faces[3][2]; //Three types of free faces, each collapsible in two directions...
	list *free_faces_bis[3][2];
	uint32_t N, i, j, k, l, l_opp, rs, cs, d, ps;
	int32_t t, shifter;
	unsigned char face, direction, stop;


	//Prepare and initialize the lists which will be used...
	for(i=0; i<3; i++)
	{
		for(j=0; j<2; j++)
		{
			free_faces[i][j] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i][j]==NULL)
			{
				fprintf(stderr, "cca_surfacic_collapse(): Error, could not create new list.\n");
				return(-1);
			}
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;


	/*****************************************
	First scan, we scan the whole image in order to
	detect the free faces...
	*******************************************/
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				//If the face XY on the current pixel is free, add it to the list
				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXY);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[0][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[0][1], l);

				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXZ);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[1][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[1][1], l);

				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FYZ);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[2][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[2][1], l);

				l++;
			}


	/***********************************
	Second phase, for each faces in a same plan,
	we collapse in two opposite direction until no more collapses are possible
	**********************************/

	//For each of the three possible directions
	stop=0;
	while(stop==0)
	{
		stop=1;

		for(i=0; i<3; i++)
		{
			if(i==0)
			{
				face=CC_FXY;
				shifter=ps;
			}
			else if (i==1)
			{
				face=CC_FXZ;
				shifter=cs;
			}
			else
			{
				face=CC_FYZ;
				shifter=1;
			}

			for(j=0; j<2; j++)
			{
				if(j==0)
					direction=CC_FACE_COLL_UP;
				else
				{
					direction=CC_FACE_COLL_DOWN;
					shifter=-shifter;
				}

				free_faces_bis[i][j] = list_newlist(MODE_FIFO, 1024);
				if(free_faces_bis[i][j]==NULL)
				{
					fprintf(stderr, "cca_surfacic_collapse: Error, could not create new list (2).\n");
					return(-1);
				}

				while(!list_isempty(free_faces[i][j]))
				{
					l=list_pop_uint32_t(free_faces[i][j]);

					//Is l really still a free face as we should have ?
					t=cca_is_face_free(cca_image, NULL, l, rs, ps, face);
					if(t==direction)
					{
						//Yes, it is, we can do the main collapse...
						cca_collapse_face(cca_image, l, rs, ps, face, direction);
						//And add the opposite face to the list bis
						if(!cca_is_face_onborder(cca_image, face, (l+shifter)%rs, ((l+shifter)%ps)/rs, (l+shifter)/ps) && cca_is_face_free(cca_image, NULL, (l+shifter), rs, ps, face))
						{
							list_push_uint32_t(free_faces_bis[i][j], (uint32_t)(l+shifter));
							stop=0;
						}
						//And collapse the edges and vertices contained in this face...
						switch(face)
						{
							case(CC_FXY):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+1, rs, ps, CC_PT, t);
								break;

							case(CC_FXZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps+1, rs, ps, CC_PT, t);
								break;

							case(CC_FYZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+ps, rs, ps, CC_PT, t);
								break;
						}
					}

					//No it is no more...
					else if(t==CC_NOT_FREE_NONEIGHBOUR || t==CC_NO_SUCH_FACE)
					{
						//Look at opposite face
						l_opp=l+shifter;
						t=cca_is_face_free(cca_image, NULL, l_opp, rs, ps, face);
						//If it is free, then it means we could have collapsed l, but we collapsed another face on the same cube...
						//Add opposite face to the liste of free faces
						if(t==direction)
						{
							list_push_uint32_t(free_faces_bis[i][j], (uint32_t)(l+shifter));
							stop=0;
						}
					}

				}

				//On détruit la liste
				list_delete(free_faces[i][j], NO_FREE_DATA);
				//Et on permute
				free_faces[i][j]=free_faces_bis[i][j];
				free_faces_bis[i][j]=NULL;
			}
		}
	}
	/***********************************
	Third phase, collapse all the remaining free faces...
	**********************************/
	//Dernier scan, pour se débrasser des volumes qu'on aurait pu oublier...
	l=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				//If the face XY on the current pixel is free, add it to the list
				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXY);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[0][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[0][1], l);

				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXZ);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[1][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[1][1], l);

				t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FYZ);
				if(t==CC_FACE_COLL_UP)
					list_push_uint32_t(free_faces[2][0], l);
				else if(t==CC_FACE_COLL_DOWN)
					list_push_uint32_t(free_faces[2][1], l);

				l++;
			}

	stop=0;
	while(stop==0)
	{
		stop=1;

		for(i=0; i<3; i++)
		{
			for(j=0; j<2; j++)
			{
				free_faces_bis[i][j] = list_newlist(MODE_FIFO, 1024);
				if(free_faces_bis[i][j]==NULL)
				{
					fprintf(stderr, "cca_surfacic_collapse(): Error, could not create new list (3).\n");
					return(-1);
				}
			}
		}

		//This time, shifter allows to know where is the volume which collpased with the free face.
		for(i=0; i<3; i++)
		{
			if(i==0)
			{
				face=CC_FXY;
				shifter=-ps;
			}
			else if (i==1)
			{
				face=CC_FXZ;
				shifter=-cs;
			}
			else
			{
				face=CC_FYZ;
				shifter=-1;
			}

			for(j=0; j<2; j++)
			{
				if(j==0)
				{
					direction=CC_FACE_COLL_UP;
					shifter=0;
				}
				else
					direction=CC_FACE_COLL_DOWN;

				while(!list_isempty(free_faces[i][j]))
				{
					l=list_pop_uint32_t(free_faces[i][j]);

					//Is l really still a free face as we should have ?
					t=cca_is_face_free(cca_image, NULL, l, rs, ps, face);
					if(t==direction)
					{
						//Yes, it is, we can do the main collapse...
						cca_collapse_face(cca_image, l, rs, ps, face, direction);

						//And collapse the edges and vertices contained in this face...
						switch(face)
						{
							case(CC_FXY):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+1, rs, ps, CC_PT, t);

								break;

							case(CC_FXZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps+1, rs, ps, CC_PT, t);

								break;

							case(CC_FYZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+ps, rs, ps, CC_PT, t);

								break;
						}

						l=l+shifter;
						//Finally, add all the faces of the volume which are free to the list of free faces...
						if(!cca_is_face_onborder(cca_image, CC_FXY, l%rs, (l%ps)/rs, l/ps) && cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXY))
						{
							list_push_uint32_t(free_faces_bis[0][1], l);
							stop=0;
						}

						if(!cca_is_face_onborder(cca_image, CC_FXZ, l%rs, (l%ps)/rs, l/ps) && cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FXZ))
						{
							list_push_uint32_t(free_faces_bis[1][1], l);
							stop=0;
						}

						if(!cca_is_face_onborder(cca_image, CC_FYZ, l%rs, (l%ps)/rs, l/ps) && cca_is_face_free(cca_image, NULL, l, rs, ps, CC_FYZ))
						{
							list_push_uint32_t(free_faces_bis[2][1], l);
							stop=0;
						}

						if(!cca_is_face_onborder(cca_image, CC_FXY, (l+1)%rs, ((l+1)%ps)/rs, (l+1)/ps) && cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_FXY))
						{
							list_push_uint32_t(free_faces_bis[0][0], l+1);
							stop=0;
						}

						if(!cca_is_face_onborder(cca_image, CC_FXZ, (l+rs)%rs, ((l+rs)%ps)/rs, (l+rs)/ps) && cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_FXZ))
						{
							list_push_uint32_t(free_faces_bis[1][0], l+rs);
							stop=0;
						}

						if(!cca_is_face_onborder(cca_image, CC_FYZ, (l+ps)%rs, ((l+ps)%ps)/rs, (l+ps)/ps) && cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_FYZ))
						{
							list_push_uint32_t(free_faces_bis[2][0], l+ps);
							stop=0;
						}

					}
				}
			}
		}

		for(i=0; i<3; i++)
			for(j=0; j<2; j++)
			{
				//On détruit la liste
				list_delete(free_faces[i][j], NO_FREE_DATA);

				//Et on permute
				free_faces[i][j]=free_faces_bis[i][j];
				free_faces_bis[i][j]=NULL;
			}

	}

	for(i=0; i<3; i++)
		for(j=0; j<2; j++)
			list_delete(free_faces[i][j], NO_FREE_DATA);

	return(0);
}


int32_t cca_vsa_collapse_keep_faces(struct xvimage *cca_image)
{
	list *free_faces[3][2]; //Three types of free faces, each collapsible in two directions...
	list *free_faces_bis[3][2];
	uint32_t N, i, j, l, l_opp, rs, cs, d, ps, r1, r2, c, cpt;
	int32_t shifter;
	unsigned char face, direction, stop, t;
	complexe *rvsa, *border_faces, *debug;
	struct xvimage *debug_image;


	//Prepare and initialize the lists which will be used...
	for(i=0; i<3; i++)
	{
		for(j=0; j<2; j++)
		{
			free_faces[i][j] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i][j]==NULL)
			{
				fprintf(stderr, "cca_vsa_collapse_keep_faces(): Error, could not create new list.\n");
				return(-1);
			}
		}
	}

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;

	debug=complexe_new_complexe();


	/*****************************************
	First scan, we compute the object's VSA and decide which
	free faces we are going to collapse in the object...
	*******************************************/
	border_faces=cca_compute_border_faces(cca_image);
	if(border_faces==NULL)
	{
		fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_compute_border_faces failed.\n");
		return (-1);
	}

	rvsa=cca_compute_vise_surfacic_axis_internal(cca_image, 0, border_faces);
	if(rvsa==NULL)
	{
		fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_compute_vise_surfacic_axis failed.\n");
		return (-1);
	}

	freeimage(cca_image);
	cca_image=readimage((char*)"curved_pipe.cc");


	for(i=0; i<rvsa->num_fxy; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FXY, border_faces, rvsa->tab_fxy[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed.\n");
			return(-1);
		}

		c=rvsa->tab_fxy[i]%ps;
		r1=r1*ps+c;
		r2=r2*ps+c;

		complexe_add_element(debug, r1, CC_FXY);
		complexe_add_element(debug, r2, CC_FXY);

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FXY);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[0][0], r1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[0][1], r1);

		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FXY);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[0][0], r2);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[0][1], r2);
	}

	for(i=0; i<rvsa->num_fxz; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FXZ, border_faces, rvsa->tab_fxz[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed (2).\n");
			return(-1);
		}

		c=rvsa->tab_fxz[i]- ((rvsa->tab_fxz[i]%ps)/rs)*rs;
		r1=r1*rs+c;
		r2=r2*rs+c;
		complexe_add_element(debug, r1, CC_FXZ);
		complexe_add_element(debug, r2, CC_FXZ);

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FXZ);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[1][0], r1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[1][1], r1);

		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FXZ);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[1][0], r2);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[1][1], r2);
	}

	for(i=0; i<rvsa->num_fyz; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FYZ, border_faces, rvsa->tab_fyz[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed (3).\n");
			return(-1);
		}

		c=rvsa->tab_fyz[i]- rvsa->tab_fyz[i]%rs;
		r1=r1+c;
		r2=r2+c;

		complexe_add_element(debug, r1, CC_FYZ);
		complexe_add_element(debug, r2, CC_FYZ);

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FYZ);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[2][0], r1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[2][1], r1);

		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FYZ);
		if(t==CC_FACE_COLL_UP)
			list_push_uint32_t(free_faces[2][0], r2);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_uint32_t(free_faces[2][1], r2);
	}

	printf("Bouh3\n");

	complexe_free_complexe(rvsa);
	complexe_free_complexe(border_faces);

	debug_image=complexe_to_cca(debug, rs, cs, d);
	//writeimage(debug_image, "debug_border.cc");
	freeimage(debug_image);


	/***********************************
	Second phase, for each faces in a same plan,
	we collapse in two opposite direction until no more collapses are possible
	**********************************/

	//For each of the three possible directions
	stop=0;
	cpt=0;
	while(stop==0)
	{
		stop=1;

		for(i=0; i<3; i++)
		{
			if(i==0)
			{
				face=CC_FXY;
				shifter=ps;
			}
			else if (i==1)
			{
				face=CC_FXZ;
				shifter=cs;
			}
			else
			{
				face=CC_FYZ;
				shifter=1;
			}

			for(j=0; j<2; j++)
			{
				if(j==0)
					direction=CC_FACE_COLL_UP;
				else
				{
					direction=CC_FACE_COLL_DOWN;
					shifter=-shifter;
				}

				free_faces_bis[i][j] = list_newlist(MODE_FIFO, 1024);
				if(free_faces_bis[i][j]==NULL)
				{
					fprintf(stderr, "cca_vsa_collapse_keep_faces: Error, could not create new list (2).\n");
					return(-1);
				}

				while(!list_isempty(free_faces[i][j]))
				{
					l=list_pop_uint32_t(free_faces[i][j]);

					//Is l really still a free face as we should have ?
					t=cca_is_face_free(cca_image, NULL, l, rs, ps, face);
					if(t==direction)
					{
						//Yes, it is, we can do the main collapse...
						cca_collapse_face(cca_image, l, rs, ps, face, direction);
						//And add the opposite face to the list bis
						if(!cca_is_face_onborder(cca_image, face, (l+shifter)%rs, ((l+shifter)%ps)/rs, (l+shifter)/ps) && cca_is_face_free(cca_image, NULL, (l+shifter), rs, ps, face))
						{
							list_push_uint32_t(free_faces_bis[i][j], (uint32_t)(l+shifter));
							stop=0;
						}
						//And collapse the edges and vertices contained in this face...
						switch(face)
						{
							case(CC_FXY):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+1, rs, ps, CC_PT, t);
								break;

							case(CC_FXZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AX);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AX, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps+1, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps+1, rs, ps, CC_PT, t);
								break;

							case(CC_FYZ):
								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AZ);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_AZ, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AY);
								if(t>=CC_EDGE_COLL_DIR1)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_AY, t);

								t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

								t=cca_is_face_free(cca_image, NULL, l+rs+ps, rs, ps, CC_PT);
								if(t>=CC_VERT_COLL_XUP)
									cca_collapse_face(cca_image, l+rs+ps, rs, ps, CC_PT, t);
								break;
						}
					}

					//No it is no more...
					else if(t==CC_NOT_FREE_NONEIGHBOUR || t==CC_NO_SUCH_FACE)
					{
						//Look at opposite face
						l_opp=l+shifter;
						t=cca_is_face_free(cca_image, NULL, l_opp, rs, ps, face);
						//If it is free, then it means we could have collapsed l, but we collapsed another face on the same cube...
						//Add opposite face to the liste of free faces
						if(t==direction)
						{
							list_push_uint32_t(free_faces_bis[i][j], (uint32_t)(l+shifter));
							stop=0;
						}
					}

				}

				//On détruit la liste
				list_delete(free_faces[i][j], NO_FREE_DATA);
				//Et on permute
				free_faces[i][j]=free_faces_bis[i][j];
				free_faces_bis[i][j]=NULL;
			}
		}
		cpt++;
		fprintf(stdout, "%d\n", cpt);
		if(cpt==1)
		{
			//writeimage(cca_image, "debug1.cc");
		}
	}


	for(i=0; i<3; i++)
		for(j=0; j<2; j++)
			list_delete(free_faces[i][j], NO_FREE_DATA);


	//Return only the faces which are no more included inside a volume...
/*	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(cca_image)[i] & CC_VOL)!= 0)
		{
			UCHARDATA(cca_image)[i] = UCHARDATA(cca_image)[i] - (UCHARDATA(cca_image)[i] & (CC_VOL | CC_FXY | CC_FYZ | CC_FXZ));
			if(i<N-1)
			{
				UCHARDATA(cca_image)[i+1] = UCHARDATA(cca_image)[i+1] - (UCHARDATA(cca_image)[i+1] & CC_FYZ);
				if(i<N-rs)
				{
					UCHARDATA(cca_image)[i+rs] = UCHARDATA(cca_image)[i+rs] - (UCHARDATA(cca_image)[i+rs] & CC_FXZ);
					if(i<N-ps)
						UCHARDATA(cca_image)[i+ps] = UCHARDATA(cca_image)[i+ps] - (UCHARDATA(cca_image)[i+ps] & CC_FXY);
				}
			}
		}
		UCHARDATA(cca_image)[i] = UCHARDATA(cca_image)[i] - (UCHARDATA(cca_image)[i] & (CC_AX | CC_AY | CC_AZ | CC_PT));
	}*/

	printf("end\n");
	return(0);
}


int32_t cca_perso(struct xvimage *cca_image, complexe *rvsa, struct xvimage *rvsa_image)
{
	list *free_faces[3][2]; //Three types of free faces, each collapsible in two directions...
	list *free_faces_bis[3][2];
	uint32_t N, i, j, l, l_opp, rs, cs, d, ps, r1, r2, c, cmax, deep;
	int32_t shifter;
	unsigned char face, direction, stop, t;
	complexe *border_faces;
	face_deep *d1;

	cmax=0;

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);
	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=cs*rs;

	//Prepare and initialize the lists which will be used...
	for(i=0; i<3; i++)
	{
		for(j=0; j<2; j++)
		{
			free_faces[i][j] = list_newlist(MODE_FIFO, 1024);
			if(free_faces[i][j]==NULL)
			{
				fprintf(stderr, "cca_vsa_collapse_keep_faces(): Error, could not create new list.\n");
				return(-1);
			}
		}
	}

	border_faces=cca_compute_border_faces(cca_image);
	if(border_faces==NULL)
	{
		fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_compute_border_faces failed.\n");
		return (-1);
	}

	for(i=0; i<rvsa->num_fxy; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FXY, border_faces, rvsa->tab_fxy[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed.\n");
			return(-1);
		}

		c=rvsa->tab_fxy[i]%ps;
		deep=r2-r1;
		r1=r1*ps+c;
		r2=r2*ps+c;
		c=deep;

		if (c>cmax)
			cmax=c;

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FXY);
		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r1;
		d1->deepness=c;

		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[0][0], d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[0][1], d1);

		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r2;
		d1->deepness=c;
		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FXY);
		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[0][0], d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[0][1], d1);
	}

	for(i=0; i<rvsa->num_fxz; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FXZ, border_faces, rvsa->tab_fxz[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed (2).\n");
			return(-1);
		}

		c=rvsa->tab_fxz[i]- ((rvsa->tab_fxz[i]%ps)/rs)*rs;
		deep=r2-r1;
		r1=r1*rs+c;
		r2=r2*rs+c;
		c=deep;

		if (c>cmax)
			cmax=c;

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FXZ);
		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r1;
		d1->deepness=c;

		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[1][0], (void*)d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[1][1], (void*)d1);

		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FXZ);
		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r2;
		d1->deepness=c;
		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[1][0], (void*)d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[1][1], (void*)d1);
	}

	for(i=0; i<rvsa->num_fyz; i++)
	{
		//Find the two border faces associated with this face of rvsa, and add them to the list of free faces
		//Dichotomic research on border_face in order to find the greater XY face on same column
		//than numpix and just before it (same x, y and z just over).
		if(cca_get_border_faces_parallel_to_face(CC_FYZ, border_faces, rvsa->tab_fyz[i], rs, ps, &r1, &r2)<0)
		{
			fprintf(stderr, "cca_vsa_collapse_keep_faces(): cca_get_border_faces_parallel_to_face() failed (3).\n");
			return(-1);
		}

		c=rvsa->tab_fyz[i]- rvsa->tab_fyz[i]%rs;
		deep=r2-r1;
		r1=r1+c;
		r2=r2+c;
		c=deep;

		if (c>cmax)
			cmax=c;

		//Normally, the faces on r1 and r2 are free, but we make sure of it... Moreover, the test will give us the direction to collapse.
		t=cca_is_face_free(cca_image, NULL, r1, rs, ps, CC_FYZ);
		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r1;
		d1->deepness=c;

		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[2][0], (void*)d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[2][1], (void*)d1);

		t=cca_is_face_free(cca_image, NULL, r2, rs, ps, CC_FYZ);
		d1=(face_deep*)calloc(1,sizeof(face_deep));
		d1->pixnum=r2;
		d1->deepness=c;
		if(t==CC_FACE_COLL_UP)
			list_push_pointer(free_faces[2][0], (void*)d1);
		else if(t==CC_FACE_COLL_DOWN)
			list_push_pointer(free_faces[2][1], (void*)d1);
	}

	stop=0;
	while(stop==0)
	{
		stop=1;

		for(i=0; i<3; i++)
		{
			if(i==0)
			{
				face=CC_FXY;
				shifter=ps;
			}
			else if (i==1)
			{
				face=CC_FXZ;
				shifter=cs;
			}
			else
			{
				face=CC_FYZ;
				shifter=1;
			}

			for(j=0; j<2; j++)
			{
				if(j==0)
					direction=CC_FACE_COLL_UP;
				else
				{
					direction=CC_FACE_COLL_DOWN;
					shifter=-shifter;
				}

				free_faces_bis[i][j] = list_newlist(MODE_FIFO, 1024);
				if(free_faces_bis[i][j]==NULL)
				{
					fprintf(stderr, "cca_vsa_collapse_keep_faces: Error, could not create new list (2).\n");
					return(-1);
				}

				while(!list_isempty(free_faces[i][j]))
				{
					d1=(face_deep*)list_pop_pointer(free_faces[i][j]);
					if(d1->deepness >= cmax)
					{
						l=d1->pixnum;
						c=d1->deepness;
						free(d1);
						//Is l really still a free face as we should have ?
						t=cca_is_face_free(cca_image, NULL, l, rs, ps, face);
						if(t==direction && (UCHARDATA(rvsa_image)[l] & face)==0)
						{
							//Yes, it is, we can do the main collapse...
							cca_collapse_face(cca_image, l, rs, ps, face, direction);
							//And add the opposite face to the list bis
							if(!cca_is_face_onborder(cca_image, face, (l+shifter)%rs, ((l+shifter)%ps)/rs, (l+shifter)/ps) && cca_is_face_free(cca_image, NULL, (l+shifter), rs, ps, face))
							{
								d1=(face_deep*)calloc(1,sizeof(face_deep));
								d1->deepness=c-1;
								d1->pixnum=l+shifter;
								list_push_pointer(free_faces_bis[i][j], (void*)d1);
								stop=0;
							}
							//And collapse the edges and vertices contained in this face...
							switch(face)
							{
								case(CC_FXY):
									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

									t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AX);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+rs, rs, ps, CC_AX, t);

									t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AY);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+1, rs, ps, CC_AY, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+rs+1, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+rs+1, rs, ps, CC_PT, t);
									break;

								case(CC_FXZ):
									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AX);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AX, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

									t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AX);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+ps, rs, ps, CC_AX, t);

									t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_AZ);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+1, rs, ps, CC_AZ, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+1, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+1, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+ps+1, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+ps+1, rs, ps, CC_PT, t);
									break;

								case(CC_FYZ):
									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AZ);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AZ, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_AY);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l, rs, ps, CC_AY, t);

									t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_AZ);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+rs, rs, ps, CC_AZ, t);

									t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_AY);
									if(t>=CC_EDGE_COLL_DIR1)
										cca_collapse_face(cca_image, l+ps, rs, ps, CC_AY, t);

									t=cca_is_face_free(cca_image, NULL, l, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+rs, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+rs, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+ps, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+ps, rs, ps, CC_PT, t);

									t=cca_is_face_free(cca_image, NULL, l+rs+ps, rs, ps, CC_PT);
									if(t>=CC_VERT_COLL_XUP)
										cca_collapse_face(cca_image, l+rs+ps, rs, ps, CC_PT, t);
									break;
							}
						}

						//No it is no more...
						else if(t==CC_NOT_FREE_NONEIGHBOUR || t==CC_NO_SUCH_FACE)
						{
							//Look at opposite face
							l_opp=l+shifter;
							t=cca_is_face_free(cca_image, NULL, l_opp, rs, ps, face);
							//If it is free, then it means we could have collapsed l, but we collapsed another face on the same cube...
							//Add opposite face to the liste of free faces
							if(t==direction)
							{
								d1=(face_deep*)calloc(1,sizeof(face_deep));
								d1->deepness=c-1;
								d1->pixnum=l+shifter;
								list_push_pointer(free_faces_bis[i][j], (void*)d1);
								stop=0;
							}
						}
					}
					else
					{
						list_push_pointer(free_faces_bis[i][j], (void*)d1);
						stop=0;
					}

				}

				//On détruit la liste
				list_delete(free_faces[i][j], NO_FREE_DATA);
				//Et on permute
				free_faces[i][j]=free_faces_bis[i][j];
				free_faces_bis[i][j]=NULL;
			}
		}
		if(cmax>0)
			cmax--;

		/*if(cmax==30)
			stop=1;*/

		fprintf(stdout, "%d\n", cmax);
	}


	for(i=0; i<3; i++)
		for(j=0; j<2; j++)
			list_delete(free_faces[i][j], NO_FREE_DATA);


	return(0);
}


void cca_check_free_faces_after_collapse(struct xvimage *cca_image, struct xvimage *inhibit, unsigned char face_collapse, unsigned char direction, list **result, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	switch(face_collapse)
	{
		case(CC_PT):
			switch(direction)
			{
				case(CC_VERT_COLL_XUP):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], pixnumber+1);
					break;

				case(CC_VERT_COLL_XDOWN):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], (pixnumber-1));
					break;

				case(CC_VERT_COLL_YUP):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], (pixnumber+rs));
					break;

				case(CC_VERT_COLL_YDOWN):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], (pixnumber-rs));
					break;

				case(CC_VERT_COLL_ZUP):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], (pixnumber+ps));
					break;

				case(CC_VERT_COLL_ZDOWN):
					if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_PT)))
						list_push_uint32_t(result[0], (pixnumber-ps));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_PT).\n");
					break;
			}
			break;

		case(CC_AZ):
			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], pixnumber);

			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], (pixnumber+ps));

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], pixnumber);

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber+ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber+1));
					break;

				case(CC_EDGE_COLL_DIR2):
					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-1+ps, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-1+ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-1));
					break;

				case(CC_EDGE_COLL_DIR3):
					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], pixnumber);

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber+ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber+rs));
					break;

				case(CC_EDGE_COLL_DIR4):
					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-rs+ps, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-rs+ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-rs));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_AZ).\n");
					break;
			}
			break;

		case(CC_AY):
			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], pixnumber);

			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], (pixnumber+rs));

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], pixnumber);

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber+rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber+1));
					break;

				case(CC_EDGE_COLL_DIR2):
					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-1+rs, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-1+rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-1));
					break;

				case(CC_EDGE_COLL_DIR3):
					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], pixnumber);

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber+rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber+ps));
					break;

				case(CC_EDGE_COLL_DIR4):
					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps+rs, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-ps+rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-ps));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_AY).\n");
					break;
			}
			break;

		case(CC_AX):
			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], pixnumber);

			if(ISFREE(CC_PT, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT)))
				list_push_uint32_t(result[0], (pixnumber+1));

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], pixnumber);

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber+1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber+rs));
					break;

				case(CC_EDGE_COLL_DIR2):
					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-rs));

					if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber-rs+1, rs, ps, CC_AY)))
						list_push_uint32_t(result[2], (pixnumber-rs+1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-rs));
					break;

				case(CC_EDGE_COLL_DIR3):
					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], pixnumber);

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber+1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber+ps));
					break;

				case(CC_EDGE_COLL_DIR4):
					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-ps));

					if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps+1, rs, ps, CC_AZ)))
						list_push_uint32_t(result[1], (pixnumber-ps+1));

					if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AX)))
						list_push_uint32_t(result[3], (pixnumber-ps));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_AX).\n");
					break;
			}
			break;

		case(CC_FXY):
			if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)))
				list_push_uint32_t(result[3], (pixnumber));

			if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)))
				list_push_uint32_t(result[2], (pixnumber));

			if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX)))
				list_push_uint32_t(result[3], (pixnumber+rs));

			if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY)))
				list_push_uint32_t(result[2], (pixnumber+1));

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber));

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber+1));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber+rs));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber+ps));
					break;

				case(CC_FACE_COLL_DOWN):
					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber-ps));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber-ps));

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1-ps, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber+1-ps));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs-ps, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber+rs-ps));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber-ps));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_FXY).\n");
					break;
			}

			break;

		case(CC_FXZ):
			if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX)))
				list_push_uint32_t(result[3], pixnumber);

			if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)))
				list_push_uint32_t(result[1], (pixnumber));

			if(ISFREE(CC_AX, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX)))
				list_push_uint32_t(result[3], (pixnumber+ps));

			if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ)))
				list_push_uint32_t(result[1], (pixnumber+1));

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], pixnumber);

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], pixnumber);

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber+1));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber+rs));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber+ps));
					break;

				case(CC_FACE_COLL_DOWN):
					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber-rs));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber-rs));

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1-rs, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber+1-rs));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber-rs));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps-rs, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber+ps-rs));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_FXZ).\n");
					break;
			}

			break;

		case(CC_FYZ):
			if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ)))
				list_push_uint32_t(result[1], pixnumber);

			if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY)))
				list_push_uint32_t(result[2], (pixnumber));

			if(ISFREE(CC_AZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ)))
				list_push_uint32_t(result[1], (pixnumber+rs));

			if(ISFREE(CC_AY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY)))
				list_push_uint32_t(result[2], (pixnumber+ps));

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], pixnumber);

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4],pixnumber);

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber+1));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber+rs));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber+ps));
					break;

				case(CC_FACE_COLL_DOWN):
					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber-1));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber-1));

					if(ISFREE(CC_FYZ, cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FYZ)))
						list_push_uint32_t(result[6], (pixnumber-1));

					if(ISFREE(CC_FXZ, cca_is_face_free(cca_image, inhibit, pixnumber+rs-1, rs, ps, CC_FXZ)))
						list_push_uint32_t(result[5], (pixnumber+rs-1));

					if(ISFREE(CC_FXY, cca_is_face_free(cca_image, inhibit, pixnumber+ps-1, rs, ps, CC_FXY)))
						list_push_uint32_t(result[4], (pixnumber+ps-1));
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown direction given, nothing done (CC_FYZ).\n");
					break;
			}
			break;

		default:
			fprintf(stderr, "cca_check_free_faces_after_collapse(): Warning, unknown face given, nothing done.\n");
			break;
	}
}


//Same than cca_check_free_faces_after_collapse, but with a directional list of free faces
//And two list as parameters : one for faces inside the collapsed face, and one for faces outisde the collapsed face
void cca_check_free_faces_after_collapse_dirstrat(struct xvimage *cca_image, struct xvimage *inhibit, unsigned char face_collapse, unsigned char direction, list **insiders, list **outsiders, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	unsigned char dir;

	switch(face_collapse)
	{
		case(CC_PT):
			switch(direction)
			{
				case(CC_VERT_COLL_XUP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber+1);
					break;

				case(CC_VERT_COLL_XDOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber-1);
					break;

				case(CC_VERT_COLL_YUP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber+rs);
					break;

				case(CC_VERT_COLL_YDOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber-rs);
					break;

				case(CC_VERT_COLL_ZUP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber+ps);
					break;

				case(CC_VERT_COLL_ZDOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_PT);
					if(ISFREE(CC_PT, dir))
						cca_add_free_face_to_list(outsiders, CC_PT, dir, pixnumber-ps);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_PT).\n");
					break;
			}
			break;

		case(CC_AZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber+ps);

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber+ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber+1);
					break;

				case(CC_EDGE_COLL_DIR2):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1+ps, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-1+ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-1);
					break;

				case(CC_EDGE_COLL_DIR3):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber+ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber+rs);
					break;

				case(CC_EDGE_COLL_DIR4):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs+ps, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-rs+ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-rs);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_AZ).\n");
					break;
			}
			break;

		case(CC_AY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber+rs);

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber+1);
					break;

				case(CC_EDGE_COLL_DIR2):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1+rs, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-1+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-1);
					break;

				case(CC_EDGE_COLL_DIR3):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber+ps);
					break;

				case(CC_EDGE_COLL_DIR4):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps+rs, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-ps+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-ps);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_AY).\n");
					break;
			}
			break;

		case(CC_AX):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_PT);
			if(ISFREE(CC_PT, dir))
				cca_add_free_face_to_list(insiders, CC_PT, dir, pixnumber+1);

			switch(direction)
			{
				case(CC_EDGE_COLL_DIR1):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber+rs);
					break;

				case(CC_EDGE_COLL_DIR2):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs+1, rs, ps, CC_AY);
					if(ISFREE(CC_AY, dir))
						cca_add_free_face_to_list(outsiders, CC_AY, dir, pixnumber-rs+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-rs);
					break;

				case(CC_EDGE_COLL_DIR3):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber+ps);
					break;

				case(CC_EDGE_COLL_DIR4):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps+1, rs, ps, CC_AZ);
					if(ISFREE(CC_AZ, dir))
						cca_add_free_face_to_list(outsiders, CC_AZ, dir, pixnumber-ps+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_AX);
					if(ISFREE(CC_AX, dir))
						cca_add_free_face_to_list(outsiders, CC_AX, dir, pixnumber-ps);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_AX).\n");
					break;
			}
			break;

		case(CC_FXY):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_add_free_face_to_list(insiders, CC_AX, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_add_free_face_to_list(insiders, CC_AY, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_add_free_face_to_list(insiders, CC_AX, dir, pixnumber+rs);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_add_free_face_to_list(insiders, CC_AY, dir, pixnumber+1);

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber+ps);
					break;

				case(CC_FACE_COLL_DOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1-ps, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber+1-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs-ps, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber+rs-ps);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-ps, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber-ps);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_FXY).\n");
					break;
			}

			break;

		case(CC_FXZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_add_free_face_to_list(insiders, CC_AX, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_add_free_face_to_list(insiders, CC_AZ, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AX);
			if(ISFREE(CC_AX, dir))
				cca_add_free_face_to_list(insiders, CC_AX, dir, pixnumber+ps);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_add_free_face_to_list(insiders, CC_AZ, dir, pixnumber+1);

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber+ps);
					break;

				case(CC_FACE_COLL_DOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1-rs, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber+1-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-rs, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber-rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps-rs, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber+ps-rs);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_FXZ).\n");
					break;
			}

			break;

		case(CC_FYZ):
			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_add_free_face_to_list(insiders, CC_AZ, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_add_free_face_to_list(insiders, CC_AY, dir, pixnumber);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_AZ);
			if(ISFREE(CC_AZ, dir))
				cca_add_free_face_to_list(insiders, CC_AZ, dir, pixnumber+rs);

			dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_AY);
			if(ISFREE(CC_AY, dir))
				cca_add_free_face_to_list(insiders, CC_AY, dir, pixnumber+ps);

			switch(direction)
			{
				case(CC_FACE_COLL_UP):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+1, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber+1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber+rs);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber+ps);
					break;

				case(CC_FACE_COLL_DOWN):
					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber-1, rs, ps, CC_FYZ);
					if(ISFREE(CC_FYZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FYZ, dir, pixnumber-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+rs-1, rs, ps, CC_FXZ);
					if(ISFREE(CC_FXZ, dir))
						cca_add_free_face_to_list(outsiders, CC_FXZ, dir, pixnumber+rs-1);

					dir=cca_is_face_free(cca_image, inhibit, pixnumber+ps-1, rs, ps, CC_FXY);
					if(ISFREE(CC_FXY, dir))
						cca_add_free_face_to_list(outsiders, CC_FXY, dir, pixnumber+ps-1);
					break;

				default:
					fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown direction given, nothing done (CC_FYZ).\n");
					break;
			}
			break;

		default:
			fprintf(stderr, "cca_check_free_faces_after_collapse_dirstrat(): Warning, unknown face given, nothing done.\n");
			break;
	}
}


void cca_check_free_faces_in_cube(struct xvimage *cca_image, struct xvimage *inhibit, list **result, uint32_t pixnumber, uint32_t rs, uint32_t ps)
{
	if( (UCHARDATA(cca_image)[pixnumber]&CC_VOL)!=0 && (inhibit!=NULL && (UCHARDATA(inhibit)[pixnumber]&CC_VOL)==0) )
	{
		if(cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXY)==CC_FACE_COLL_UP)
			list_push_uint32_t(result[POS_FXY], pixnumber);

		if(cca_is_face_free(cca_image, inhibit, pixnumber+ps, rs, ps, CC_FXY)==CC_FACE_COLL_DOWN)
			list_push_uint32_t(result[POS_FXY], (pixnumber+ps));

		if(cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FXZ)==CC_FACE_COLL_UP)
			list_push_uint32_t(result[POS_FXZ], pixnumber);

		if(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FXZ)==CC_FACE_COLL_DOWN)
			list_push_uint32_t(result[POS_FXZ], (pixnumber+rs));

		if(cca_is_face_free(cca_image, inhibit, pixnumber, rs, ps, CC_FYZ)==CC_FACE_COLL_UP)
			list_push_uint32_t(result[POS_FYZ], pixnumber);

		if(cca_is_face_free(cca_image, inhibit, pixnumber+rs, rs, ps, CC_FYZ)==CC_FACE_COLL_DOWN)
			list_push_uint32_t(result[POS_FYZ], (pixnumber+1));
	}
}
