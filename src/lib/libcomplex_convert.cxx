//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>
#include <mcimage.h>
#include <mcliste.h>
#include <mcrbt.h>
#include <mcpolygons.h>

//Librairie perso
#include <libcomplex.h>
#include <libeps.h>
#include <libinventor.h>
#include <libvskel.h>
#include <libvrml.h>


/*!	\brief Converts a PGM image (given as a struct xvimage*) to a cubical complex format.
	WARNING : this function only works for binary 3d images where the background is 0.

	\param image The input image to convert. WARNING : the image will be changed, no new image is allocated by the process.

	\return Negative integer if error.

	\ingroup complex

	\author John Chaussard
*/
int32_t pgm_to_cca(struct xvimage *image)
{
	int32_t i,j, k, ps, rs, cs, d, N, new_rs, new_cs, new_d, new_ps, new_N;
	unsigned char t;

	if (image==NULL || datatype(image)!=VFF_TYP_1_BYTE || depth(image)<1)
	{
		fprintf(stderr, "convert_to_cubcomplex: Error - image must be a 1 byte per pixel image.\n");
		return(-1);
	}

	cs = colsize(image);
	rs = rowsize(image);
	d = depth(image);
	ps = rs*cs;
	N = ps*d-1;

	//TODO : don't use the realloc which will, for a short moment, allocate two times the image in the memory.
	//It would be better to use a personal function to read in the file and allocate a big image right at the beginning

	//First, we change the size of the image in order to add one to each of its dimensions
	//We have to do this because each pixel will contain information for on of its edge, three vertices, and thee faces...
	//We need to create more pixels to have information on the border pixels
	colsize(image)++;
	rowsize(image)++;

	new_rs=rowsize(image);
	new_cs=colsize(image);
	new_ps=colsize(image)*rowsize(image);
	depth(image)++;
	new_d=depth(image);


	new_N=new_ps*depth(image);

	//Then, we make the array containing the pixels bigger
	image->image_data = (void*)realloc(UCHARDATA(image), (colsize(image)) * (rowsize(image)) * (depth(image)) * sizeof(char) );

	//Initializing the newly allocated memory
	for(i=N+1; i<new_N; i++)
	{
		UCHARDATA(image)[i]=0;
	}

	//Then, we shift the voxels, from end to beginning
	for(k=d-1; k>=0; k--)
		for(j=cs-1; j>=0; j--)
			for(i=rs-1; i>=0; i--)
			{
				t=UCHARDATA(image)[N];
				UCHARDATA(image)[N--]=0;
				UCHARDATA(image)[i+j*new_rs+k*new_ps]=t;
			}


	//Finally, we convert the image into a cca format
	//Again, no need to go on some borders of the image as these borders were added and contain no white voxel
	//Each pixel will be coded on one byte. Signification of the bytes from LSB to MSB:
	//The voxel contains its top left deepless vertex.
	//The voxel contains its edge propagating in the z dimension
	//The voxel contains its edge propagating in the y dimension
	//The voxel contains its edge propagating in the x dimension
	//The voxel contains its deepless face XY
	//The voxel contains its top XZ face
	//The voxel contains its most left YZ face
	//The voxel contains it volume (here, we dont't care at all)
	if(d>1)
	{
		for(k=d-1; k>=0; k--)
			for(j=cs-1; j>=0; j--)
				for(i=rs-1; i>=0; i--)
				{
					if (UCHARDATA(image)[i+j*new_rs+k*new_ps] != 0)
					{
						//We add a cube
						cca_add_complexface(image, CC_VOL, (i+j*new_rs+k*new_ps), new_rs, new_ps);
					}
				}
	}
	else
	{
		for(j=cs-1; j>=0; j--)
			for(i=rs-1; i>=0; i--)
			{
				if (UCHARDATA(image)[i+j*new_rs] != 0)
				{
					UCHARDATA(image)[i+j*new_rs]=CC_FXY;
					//We add a cube
					cca_add_complexface(image, CC_FXY, (i+j*new_rs), new_rs, 0);
				}
			}
	}


	return(0);
}



struct xvimage *khalimsky3d_to_cca(struct xvimage *khal_image)
{
	uint32_t k_rs, k_cs, k_d, k_N, somme_modulo, cca_rs, cca_ps, i, j, k;
	struct xvimage *cca_image;

	if(rowsize(khal_image)%2!=1 || colsize(khal_image)%2!=1 || depth(khal_image)%2!=1)
	{
		fprintf(stderr, "khalimsky3d_to_cca() : Error, given khalimsky image does not have correct size (even size).\n");
		return(NULL);
	}

	k_rs=rowsize(khal_image);
	k_cs=colsize(khal_image);
	k_d=depth(khal_image);
	if(k_d!=1)
		cca_image=allocimage(NULL, k_rs/2+1, k_cs/2+1, k_d/2+1, VFF_TYP_1_BYTE);
	else
		cca_image=allocimage(NULL, k_rs/2+1, k_cs/2+1, 2, VFF_TYP_1_BYTE);
	if(cca_image==NULL)
	{
		fprintf(stderr, "khalimsky3d_to_cca() : Memory allocation failed.\n");
		return(NULL);
	}
	cca_rs = rowsize(cca_image);
	cca_ps = cca_rs*colsize(cca_image);

	k_N=0;
	for(k=0; k<k_d; k++)
		for(j=0; j<k_cs; j++)
			for(i=0; i<k_rs; i++)
			{
				if(UCHARDATA(khal_image)[k_N] != 0)
				{
					somme_modulo = i%2 + j%2 + k%2;

					switch(somme_modulo)
					{
						case(0):
							//We must add a vertice
							UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_PT;
							break;

						case(1):
							//We must add an edge
							if(i%2!=0)
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_AX;
							else if(j%2!=0)
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_AY;
							else
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_AZ;
							break;

						case(2):
							if(i%2==0)
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_FYZ;
							else if(j%2==0)
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_FXZ;
							else
								UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_FXY;
							break;

						case(3):
							UCHARDATA(cca_image)[(i/2)+(j/2*cca_rs)+(k/2*cca_ps)]+=CC_VOL;
							break;

						default:
							fprintf(stderr, "khalimsky3d_to_cca() : Error, this error should not happen if modulo function is correct.\n");
							freeimage(cca_image);
							return(NULL);
					}
				}
				k_N++;
			}

	return(cca_image);
}


struct xvimage *cca_to_khalimsky3d(struct xvimage *cca_image)
{
	uint32_t k_rs, k_ps, cca_N, cca_rs, cca_cs, cca_d, cca_ps, i, j, k, N_k;
	struct xvimage *khal_image;

	cca_rs = rowsize(cca_image);
	cca_cs = colsize(cca_image);
	cca_d = depth(cca_image);
	cca_ps = cca_rs*cca_cs;

	if(cca_d==2) cca_d=1;  //2d images ugly patch

	khal_image=allocimage(NULL, (cca_rs*2)-1, (cca_cs*2)-1, (cca_d*2)-1, VFF_TYP_1_BYTE);
	if(khal_image==NULL)
	{
		fprintf(stderr, "cca_to_khalimsky3d(): Memory allocation failed.\n");
		return(NULL);
	}

	k_rs=rowsize(khal_image);
	k_ps=colsize(khal_image)*k_rs;
	N_k=k_ps*depth(khal_image);

	cca_N=0;
	for(k=0; k<cca_d; k++)
		for(j=0; j<cca_cs; j++)
			for(i=0; i<cca_rs; i++)
			{
				if(UCHARDATA(cca_image)[cca_N]!=0)
				{
					if ((UCHARDATA(cca_image)[cca_N] & CC_VOL)!=0)
						UCHARDATA(khal_image)[(2*k+1)*k_ps + (2*j+1)*k_rs + (2*i+1)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_FXY)!=0)
						UCHARDATA(khal_image)[(2*k)*k_ps + (2*j+1)*k_rs + (2*i+1)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_FXZ)!=0)
						UCHARDATA(khal_image)[(2*k+1)*k_ps + (2*j)*k_rs + (2*i+1)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_FYZ)!=0)
						UCHARDATA(khal_image)[(2*k+1)*k_ps + (2*j+1)*k_rs + (2*i)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_AX)!=0)
						UCHARDATA(khal_image)[(2*k)*k_ps + (2*j)*k_rs + (2*i+1)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_AY)!=0)
						UCHARDATA(khal_image)[(2*k)*k_ps + (2*j+1)*k_rs + (2*i)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_AZ)!=0)
						UCHARDATA(khal_image)[(2*k+1)*k_ps + (2*j)*k_rs + (2*i)]=255;

					if ((UCHARDATA(cca_image)[cca_N] & CC_PT)!=0)
						UCHARDATA(khal_image)[(2*k)*k_ps + (2*j)*k_rs + (2*i)]=255;
				}

				cca_N++;
			}

	return(khal_image);
}


/*!	\brief Converts a cubical complex image into an inventor file.

	\param cca_image The input image to convert.
	\param output_file The inventor output file.
	\param activate_amira_patch In order to visualize correctly (no shift) the image in amira, activate this option.

	\return Negative integer if error.

	\ingroup complex
	\ingroup inventor

	\author John Chaussard
*/
//Scene must have MatPoint, MatEdge, MatFace and MatVolume defined
int32_t cca_to_inventor(struct xvimage *cca_image, iv_scene *scene, char point_behaviour, char amira_patch, char mode_vrml)
{
	uint32_t rs, cs, d, ps, N, l, num_pt, num_ax, num_ay, num_az, num_fxy, num_fxz, num_fyz, num_vol, num_vtx;
	//The main array which will contain all elements of the scene...
	uint32_t **all_elements;
	//The array with all the vertex of the scene
	uint32_t *list_pt;
	complexe *cpl;

	all_elements=(uint32_t**)calloc(10, sizeof(uint32_t*));
	if(all_elements==NULL)
	{
		fprintf(stderr, "cca_to_inventor(): Memory allocation failed.\n");
		return(-1);
	}

	rs=rowsize(cca_image);
	cs=colsize(cca_image);
	d=depth(cca_image);
	ps=rs*cs;
	N=ps*d;

	//**************************************************
	//Second part, we scan the image and list all its elements...
	//Before, we list all the vertices of the scene
	//*************************************************
	num_vtx=1; //We have to put to 1 in order to see, if list_pt (under) is null, check if it is because no vertex or because error
	cpl=complexe_new_complexe();
	if(cpl==NULL)
	{
		fprintf(stderr, "cca_to_inventor(): Error when creating complex structure.\n");
		return(-1);
	}
	if(cca_scan_all_vertex_into_complex(cca_image, cpl)<0)
	{
		fprintf(stderr, "cca_to_inventor(): Error when listing vertex.\n");
		return(-1);
	}

	list_pt=cpl->tab_pt_obj;
	num_vtx=cpl->num_pt_obj;

	num_pt=0;num_ax=0;num_ay=0;num_az=0;num_fxy=0;num_fyz=0;num_fxz=0;num_vol=0;

	//Scan to list all the elements of the scene...
	for(l=0; l<N; l++)
	{
		if(UCHARDATA(cca_image)[l]>0)
		{
			//During this scan, if the element is a volume, we remove its faces also... In order to see the volume and not the faces.
			if((UCHARDATA(cca_image)[l] & CC_VOL)!=0)
			{
				num_vol++;
				//We remove the six faces of the voxel...
				UCHARDATA(cca_image)[l] = (UCHARDATA(cca_image)[l] | CC_FXY | CC_FYZ | CC_FXZ) - CC_FXY - CC_FYZ - CC_FXZ;
				UCHARDATA(cca_image)[l+1] = (UCHARDATA(cca_image)[l+1] | CC_FYZ) - CC_FYZ;
				UCHARDATA(cca_image)[l+rs] = (UCHARDATA(cca_image)[l+rs] | CC_FXZ) - CC_FXZ;
				UCHARDATA(cca_image)[l+ps] = (UCHARDATA(cca_image)[l+ps] | CC_FXY) - CC_FXY;
			}

			if((UCHARDATA(cca_image)[l] & CC_AX)!=0) num_ax++;
			if((UCHARDATA(cca_image)[l] & CC_AY)!=0) num_ay++;
			if((UCHARDATA(cca_image)[l] & CC_AZ)!=0) num_az++;
			if((UCHARDATA(cca_image)[l] & CC_FXY)!=0) num_fxy++;
			if((UCHARDATA(cca_image)[l] & CC_FXZ)!=0) num_fxz++;
			if((UCHARDATA(cca_image)[l] & CC_FYZ)!=0) num_fyz++;
			if((UCHARDATA(cca_image)[l] & CC_PT)!=0) num_pt++;

		}
	}

	//all_elements array will list all the elements of the scene...
	//We will use a second point array if we want to display only some points of the image...
	if(point_behaviour==DRAW_PT && num_pt>0)
	{
		all_elements[POS_PT]=(uint32_t*)calloc(num_pt, sizeof(uint32_t));
		if(all_elements[POS_PT]==NULL)
		{
			fprintf(stderr, "cca_to_inventor(): Memory allocation failed (2).\n");
			return(-1);
		}
	}

	if(num_ax>0) all_elements[POS_AX]=(uint32_t*)calloc(num_ax, sizeof(uint32_t));
	if(num_ay>0) all_elements[POS_AY]=(uint32_t*)calloc(num_ay, sizeof(uint32_t));
	if(num_az>0) all_elements[POS_AZ]=(uint32_t*)calloc(num_az, sizeof(uint32_t));
	if(num_fxy>0) all_elements[POS_FXY]=(uint32_t*)calloc(num_fxy, sizeof(uint32_t));
	if(num_fxz>0) all_elements[POS_FXZ]=(uint32_t*)calloc(num_fxz, sizeof(uint32_t));
	if(num_fyz>0) all_elements[POS_FYZ]=(uint32_t*)calloc(num_fyz, sizeof(uint32_t));
	//For each volume, two faces to draw in one of the three directions...
	if(num_vol>0) all_elements[POS_VXY]=(uint32_t*)calloc(num_vol*2, sizeof(uint32_t));
	if(num_vol>0) all_elements[POS_VXZ]=(uint32_t*)calloc(num_vol*2, sizeof(uint32_t));
	if(num_vol>0) all_elements[POS_VYZ]=(uint32_t*)calloc(num_vol*2, sizeof(uint32_t));

	if(	(all_elements[POS_AX]==NULL && num_ax>0) || (all_elements[POS_AY]==NULL && num_ay>0) || (all_elements[POS_AZ]==NULL && num_az>0) ||
		(all_elements[POS_FXY]==NULL && num_fxy>0) || (all_elements[POS_FXZ]==NULL && num_fxz>0) || (all_elements[POS_FYZ]==NULL && num_fyz>0) ||
		(all_elements[POS_VXY]==NULL && num_vol>0) || (all_elements[POS_VXZ]==NULL && num_vol>0) || (all_elements[POS_VYZ]==NULL && num_vol>0) )
	{
		fprintf(stderr, "cca_to_inventor(): Memory allocation failed (3).\n");
		return(-1);
	}

	//Now we list all elements of the scene
	num_pt=0;num_ax=0;num_ay=0;num_az=0;num_fxy=0;num_fyz=0;num_fxz=0;num_vol=0;
	for(l=0; l<N; l++)
	{
		if(UCHARDATA(cca_image)[l]>0)
		{
			if(((UCHARDATA(cca_image)[l] & CC_PT)!=0) && point_behaviour==DRAW_PT) all_elements[POS_PT][num_pt++]=l; //Use a second point array only if we want to draw some points
			if((UCHARDATA(cca_image)[l] & CC_AX)!=0) all_elements[POS_AX][num_ax++]=l;
			if((UCHARDATA(cca_image)[l] & CC_AY)!=0) all_elements[POS_AY][num_ay++]=l;
			if((UCHARDATA(cca_image)[l] & CC_AZ)!=0) all_elements[POS_AZ][num_az++]=l;
			if((UCHARDATA(cca_image)[l] & CC_FXY)!=0) all_elements[POS_FXY][num_fxy++]=l;
			if((UCHARDATA(cca_image)[l] & CC_FXZ)!=0) all_elements[POS_FXZ][num_fxz++]=l;
			if((UCHARDATA(cca_image)[l] & CC_FYZ)!=0) all_elements[POS_FYZ][num_fyz++]=l;
			if((UCHARDATA(cca_image)[l] & CC_VOL)!=0)
			{
				all_elements[POS_VXY][num_vol]=l;all_elements[POS_VXZ][num_vol]=l;all_elements[POS_VYZ][num_vol++]=l;
				all_elements[POS_VXY][num_vol]=l+ps;all_elements[POS_VXZ][num_vol]=l+rs;all_elements[POS_VYZ][num_vol++]=l+1;
			}
		}
	}

	//**************************************************
	//Third part
	//We have all the elements, let's draw them !
	//**************************************************
	inventor_new_object(scene);

	//Put, in the iv file, all the points of the object...
	if(num_vtx>0)
	{
		inventor_declare_points(scene, (char*)"Vertex", list_pt, num_vtx, rs, ps, amira_patch);

		if(point_behaviour==DRAW_ALL && num_vtx>0)
		{
			inventor_call_defined(scene, (char*)"MatPoint");
			inventor_use_previously_defined_points(scene);
		}
	}

	//Then, draw all the edges, faces, and volumes
	//For the moment, only faces are implemented
	if(mode_vrml==1)
	{
		inventor_call_defined(scene, (char*)"MatFace");

		inventor_init_faceset(scene);

		if(num_fxy>0)
		{

			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FXY], num_fxy, CC_FXY, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing XY faces.\n");
				return(-1);
			}
			free(all_elements[POS_FXY]);
		}

		if(num_fxz>0)
		{
			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FXZ], num_fxz, CC_FXZ, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing XZ faces.\n");
				return(-1);
			}
			free(all_elements[POS_FXZ]);
		}

		if(num_fyz>0)
		{
			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FYZ], num_fyz, CC_FYZ, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing YZ faces.\n");
				return(-1);
			}
			free(all_elements[POS_FYZ]);
		}

		inventor_close_faceset(scene);
	}
	else
	{
		if(num_ax+num_ay+num_az > 0)
		{
			inventor_call_defined(scene, (char*)"MatEdge");

			inventor_init_edgeset(scene);

			if(num_ax>0)
			{
				if (inventor_draw_cubicalcomplex_edges(scene, list_pt, num_vtx, all_elements[POS_AX], num_ax, CC_AX, rs, ps)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing X edges.\n");
					return(-1);
				}
				free(all_elements[POS_AX]);
			}

			if(num_ay>0)
			{
				if (inventor_draw_cubicalcomplex_edges(scene, list_pt, num_vtx, all_elements[POS_AY], num_ay, CC_AY, rs, ps)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing Y edges.\n");
					return(-1);
				}
				free(all_elements[POS_AY]);
			}

			if(num_az>0)
			{
				if (inventor_draw_cubicalcomplex_edges(scene, list_pt, num_vtx, all_elements[POS_AZ], num_az, CC_AZ, rs, ps)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing Z edges.\n");
					return(-1);
				}
				free(all_elements[POS_AZ]);
			}

			inventor_close_edgeset(scene);
		}

		if(num_fxy+num_fxz+num_fyz > 0)
		{
			inventor_call_defined(scene, (char*)"MatFace");

			inventor_init_faceset(scene);

			if(num_fxy>0)
			{
				if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FXY], num_fxy, CC_FXY, rs, ps, 0)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing XY faces.\n");
					return(-1);
				}
				free(all_elements[POS_FXY]);
			}

			if(num_fxz>0)
			{
				if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FXZ], num_fxz, CC_FXZ, rs, ps, 0)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing XZ faces.\n");
					return(-1);
				}
				free(all_elements[POS_FXZ]);
			}

			if(num_fyz>0)
			{
				if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_FYZ], num_fyz, CC_FYZ, rs, ps, 0)!=0)
				{
					fprintf(stderr, "cca_to_inventor(): Error when drawing YZ faces.\n");
					return(-1);
				}
				free(all_elements[POS_FYZ]);
			}

			inventor_close_faceset(scene);
		}

		if(num_vol>0)
		{
			inventor_init_faceset(scene);

			inventor_call_defined(scene, (char*)"MatVolume");

			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_VXY], num_vol, CC_FXY, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing cube's XY faces.\n");
				return(-1);
			}
			free(all_elements[POS_VXY]);

			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_VXZ], num_vol, CC_FXZ, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing cube's XZ faces.\n");
				return(-1);
			}
			free(all_elements[POS_VXZ]);

			if (inventor_draw_cubicalcomplex_faces(scene, list_pt, num_vtx, all_elements[POS_VYZ], num_vol, CC_FYZ, rs, ps, 0)!=0)
			{
				fprintf(stderr, "cca_to_inventor(): Error when drawing cube's YZ faces.\n");
				return(-1);
			}
			free(all_elements[POS_VYZ]);

			inventor_close_faceset(scene);
		}
	}


	if(point_behaviour==DRAW_PT && num_pt>0)
	{
		inventor_declare_points(scene, NULL, all_elements[POS_PT], num_pt, rs, ps, amira_patch);
		free(all_elements[POS_PT]);
		inventor_call_defined(scene, (char*)"MatPoint");
		inventor_use_previously_defined_points(scene);
	}

	inventor_close_object(scene);

	free(all_elements);
	complexe_free_complexe(cpl);

	return(0);
}



int32_t cca_to_eps(struct xvimage *cca_image, eps_scene *scene)
{
	uint32_t rs, n, N;

	if(depth(cca_image)!=2)
	{
		fprintf(stderr, "cca_to_eps(): Only 2d file can be converted to eps.\n");
		return(-1);
	}

	eps_init_scene(scene);
	eps_define_square(scene, 0.71, 0.71, 0.71, (char*)"square");
	eps_define_hline(scene, 0.0, 0.0, 0.0, (char*)"hline");
	eps_define_vline(scene, 0.0, 0.0, 0.0, (char*)"vline");
	eps_define_point(scene, 0.0, 0.0, 0.0, (char*)"point");


	rs=rowsize(cca_image);
	N=colsize(cca_image)*rs;

	for(n=0; n<N; n++)
	{
		if( (UCHARDATA(cca_image)[n] & CC_FXY) != 0)
		{
			eps_draw_element(scene, (char*)"square", n%rs, n/rs);
		}

		if( (UCHARDATA(cca_image)[n] & CC_AX) != 0)
		{
			eps_draw_element(scene, (char*)"hline", n%rs, n/rs);
		}

		if( (UCHARDATA(cca_image)[n] & CC_AY) != 0)
		{
			eps_draw_element(scene, (char*)"vline", n%rs, n/rs);
		}

		if( (UCHARDATA(cca_image)[n] & CC_PT) != 0)
		{
			eps_draw_element(scene, (char*)"point", n%rs, n/rs);
		}
	}

	eps_close_scene(scene);

	return(0);
}


//Draws a complex in an inventor file... The elements are drawn refered to the tab_pt_scene array.
//If this array is left to NULL, then the complex's points are used.
//All elements of the complex are declared and drawn
//The scene points are neither declared or drawn (do it yourself)
uint32_t complexe_to_inventor(iv_scene* scene, complexe* object, uint32_t num_pt_scene, uint32_t* tab_pt_scene, uint32_t rs, uint32_t ps, char amira_patch, char *name_matpt, char *name_matedge, char *name_matface, char *name_matvol)
{
	uint32_t i, num_pt, start_tab_pt;

	if(tab_pt_scene==NULL)
	{
		tab_pt_scene=object->tab_pt_obj;
		num_pt_scene=object->num_pt_obj;
	}

	if(object->num_pt_obj > 0)
	{
		inventor_declare_points(scene, NULL, object->tab_pt_obj, object->num_pt_obj, rs, ps, amira_patch);
		if(name_matpt!=NULL)
			inventor_call_defined(scene, name_matpt);
		inventor_use_previously_defined_points(scene);
	}

	if(object->num_ax + object->num_ay + object->num_az >0)
	{
		if(name_matedge != NULL)
			inventor_call_defined(scene, name_matedge);

		inventor_init_edgeset(scene);

		if(object->num_ax > 0)
		{
			if(inventor_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_ax, object->num_ax, CC_AX, rs, ps)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing ax edges.\n");
				return(-1);
			}
		}

		if(object->num_ay >0)
		{
			if(inventor_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_ay, object->num_ay, CC_AY, rs, ps)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing ay edges.\n");
				return(-1);
			}
		}

		if(object->num_az >0)
		{
			if(inventor_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_az, object->num_az, CC_AZ, rs, ps)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing az edges.\n");
				return(-1);
			}
		}

		inventor_close_edgeset(scene);
	}

	if(object->num_fxy + object->num_fxz + object->num_fyz > 0)
	{
		if(name_matface != NULL)
			inventor_call_defined(scene, name_matface);

		inventor_init_faceset(scene);

		if(object->num_fxy >0)
		{
			if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fxy, object->num_fxy, CC_FXY, rs, ps, 0)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing fxy faces.\n");
				return(-1);
			}
		}

		if(object->num_fxz >0)
		{
			if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fxz, object->num_fxz, CC_FXZ, rs, ps, 0)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing fxz faces.\n");
				return(-1);
			}
		}

		if(object->num_fyz >0)
		{
			if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fyz, object->num_fyz, CC_FYZ, rs, ps, 0)<0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing fyz faces.\n");
				return(-1);
			}
		}

		inventor_close_faceset(scene);
	}


	if(object->num_vol >0)
	{
		if(name_matvol != NULL)
			inventor_call_defined(scene, name_matvol);

		inventor_init_faceset(scene);

		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FXY, rs, ps, 0)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vxy faces.\n");
			return(-1);
		}
		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FXY, rs, ps, ps)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vxy faces.\n");
			return(-1);
		}

		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FXZ, rs, ps, 0)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vxz faces.\n");
			return(-1);
		}
		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FXZ, rs, ps, rs)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vxz faces.\n");
			return(-1);
		}

		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FYZ, rs, ps, 0)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vyz faces.\n");
			return(-1);
		}
		if(inventor_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_vol, object->num_vol, CC_FYZ, rs, ps, 1)<0)
		{
			fprintf(stderr, "inventor_draw_complexe(): Error while drawing vyz faces.\n");
			return(-1);
		}

		inventor_close_faceset(scene);
	}


	if(object->tab_edges >0)
	{
		if(name_matedge != NULL)
			inventor_call_defined(scene, name_matedge);
		inventor_init_edgeset(scene);
		for(i=0; i<object->num_edges; i++)
		{
			num_pt=complexe_get_numvertex_of_element(object, i, GC_EDGE);
			start_tab_pt=complexe_get_vertex_position_in_element(object, i, 0, GC_EDGE);
			if(inventor_draw_curve(scene, tab_pt_scene, num_pt_scene, &(object->tab_edges_pts[start_tab_pt]), num_pt) <0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing edges.\n");
				return(-1);
			}
		}
		inventor_close_edgeset(scene);
	}


	if(object->tab_faces >0)
	{
		if(name_matface != NULL)
			inventor_call_defined(scene, name_matface);

		inventor_init_faceset(scene);
		for(i=0; i<object->num_faces; i++)
		{
			num_pt=complexe_get_numvertex_of_element(object, i, GC_FACE);
			start_tab_pt=complexe_get_vertex_position_in_element(object, i, 0, GC_FACE);
			if(inventor_draw_surface(scene, tab_pt_scene, num_pt_scene, &(object->tab_faces_pts[start_tab_pt]), num_pt) <0)
			{
				fprintf(stderr, "inventor_draw_complexe(): Error while drawing faces.\n");
				return(-1);
			}
		}
		inventor_close_faceset(scene);
	}


	return(0);
}


//Draws a complex in an vskel file... The elements are drawn refered to the tab_pt_scene array.
//If this array is left to NULL, then the complex's points are used.
//All elements of the complex are declared and drawn
//The scene points are neither declared or drawn (do it yourself)
uint32_t complexe_to_vskel(iv_scene* scene, complexe* object, uint32_t num_pt_scene, uint32_t* tab_pt_scene, uint32_t rs, uint32_t ps)
{
	uint32_t i, start_tab_pt, num_pt;


	if(object->num_ax + object->num_ay + object->num_az >0)
	{
		vskel_init_edgeset(scene);

		if(object->num_ax > 0)
		{
			if(vskel_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_ax, object->num_ax, CC_AX, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing ax edges.\n");
				return(-1);
			}
		}

		if(object->num_ay >0)
		{
			if(vskel_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_ay, object->num_ay, CC_AY, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing ay edges.\n");
				return(-1);
			}
		}

		if(object->num_az >0)
		{
			if(vskel_draw_cubicalcomplex_edges(scene, tab_pt_scene, num_pt_scene, object->tab_az, object->num_az, CC_AZ, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing az edges.\n");
				return(-1);
			}
		}

		vskel_close_edgeset(scene);
	}

	if(object->num_fxy + object->num_fxz + object->num_fyz > 0)
	{
		vskel_init_faceset(scene);
		if(object->num_fxy >0)
		{
			if(vskel_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fxy, object->num_fxy, CC_FXY, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing fxy faces.\n");
				return(-1);
			}
		}

		if(object->num_fxz >0)
		{
			if(vskel_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fxz, object->num_fxz, CC_FXZ, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing fxz faces.\n");
				return(-1);
			}
		}

		if(object->num_fyz >0)
		{
			if(vskel_draw_cubicalcomplex_faces(scene, tab_pt_scene, num_pt_scene, object->tab_fyz, object->num_fyz, CC_FYZ, rs, ps)<0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing fyz faces.\n");
				return(-1);
			}
		}

		vskel_close_faceset(scene);
	}




	if(object->tab_edges >0)
	{
//		vskel_init_edgeset(scene);
		for(i=0; i<object->num_edges; i++)
		{
			num_pt=complexe_get_numvertex_of_element(object, i, GC_EDGE);
			start_tab_pt=complexe_get_vertex_position_in_element(object, i, 0, GC_EDGE);
			if(vskel_draw_curve(scene, tab_pt_scene, num_pt_scene, &(object->tab_edges_pts[start_tab_pt]), num_pt) <0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing edges.\n");
				return(-1);
			}
		}
//		vskel_close_edgeset(scene);
	}


	if(object->tab_faces >0)
	{
		vskel_init_faceset(scene);
		for(i=0; i<object->num_faces; i++)
		{
			num_pt=complexe_get_numvertex_of_element(object, i, GC_FACE);
			start_tab_pt=complexe_get_vertex_position_in_element(object, i, 0, GC_FACE);
			if(vskel_draw_surface(scene, tab_pt_scene, num_pt_scene, &(object->tab_faces_pts[start_tab_pt]), num_pt) <0)
			{
				fprintf(stderr, "complexe_to_vskel(): Error while drawing faces.\n");
				return(-1);
			}
		}
		vskel_close_faceset(scene);
	}


	return(0);
}


void complexe_add_to_cca(struct xvimage *input, complexe* object)
{
	uint32_t i;

	for(i=0; i<object->num_pt_obj; i++)
		UCHARDATA(input)[object->tab_pt_obj[i]]|=CC_PT;

	for(i=0; i<object->num_ax; i++)
		UCHARDATA(input)[object->tab_ax[i]]|=CC_AX;

	for(i=0; i<object->num_ay; i++)
		UCHARDATA(input)[object->tab_ay[i]]|=CC_AY;

	for(i=0; i<object->num_az; i++)
		UCHARDATA(input)[object->tab_az[i]]|=CC_AZ;

	for(i=0; i<object->num_fxy; i++)
		UCHARDATA(input)[object->tab_fxy[i]]|=CC_FXY;

	for(i=0; i<object->num_fxz; i++)
		UCHARDATA(input)[object->tab_fxz[i]]|=CC_FXZ;

	for(i=0; i<object->num_fyz; i++)
		UCHARDATA(input)[object->tab_fyz[i]]|=CC_FYZ;

	for(i=0; i<object->num_vol; i++)
		UCHARDATA(input)[object->tab_vol[i]]|=CC_VOL;
}


void complexe_remove_from_cca(struct xvimage *input, complexe* object)
{
	uint32_t i;

	for(i=0; i<object->num_pt_obj; i++)
		UCHARDATA(input)[object->tab_pt_obj[i]]&=255-CC_PT;

	for(i=0; i<object->num_ax; i++)
		UCHARDATA(input)[object->tab_ax[i]]&=255-CC_AX;

	for(i=0; i<object->num_ay; i++)
		UCHARDATA(input)[object->tab_ay[i]]&=255-CC_AY;

	for(i=0; i<object->num_az; i++)
		UCHARDATA(input)[object->tab_az[i]]&=255-CC_AZ;

	for(i=0; i<object->num_fxy; i++)
		UCHARDATA(input)[object->tab_fxy[i]]&=255-CC_FXY;

	for(i=0; i<object->num_fxz; i++)
		UCHARDATA(input)[object->tab_fxz[i]]&=255-CC_FXZ;

	for(i=0; i<object->num_fyz; i++)
		UCHARDATA(input)[object->tab_fyz[i]]&=255-CC_FYZ;

	for(i=0; i<object->num_vol; i++)
		UCHARDATA(input)[object->tab_vol[i]]&=255-CC_VOL;
}


struct xvimage* complexe_to_cca(complexe* object, uint32_t rs, uint32_t cs, uint32_t d)
{
	struct xvimage* result;

	result=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
	if(result==NULL)
	{
		fprintf(stderr, "complexe_to_cca(): allocimage failed.\n");
		return(NULL);
	}

	complexe_add_to_cca(result, object);

	return(result);
}


complexe* cca_to_complexe(struct xvimage* cca_image)
{
	complexe *result;
	uint32_t N, i;
	unsigned char j;

	N=rowsize(cca_image)*colsize(cca_image)*depth(cca_image);

	result=complexe_new_complexe();

	for(i=0; i<N; i++)
		if(UCHARDATA(cca_image)[i]!=0)
			for(j=CC_PT; j!=0 && j<=CC_VOL; j=j*2)  //Double check... normally, j!=0 is enough but if one day, j is no more on a byte, we would have problem...
				if( (UCHARDATA(cca_image)[i] & j) != 0)
					if( complexe_add_element(result, i, j)!= 0)
					{
						fprintf(stderr, "cca_to_complexe: function complexe_add_element failed.\n");
						return NULL;
					}

	return result;
}



complexe* cc_to_gc_only_simple_edges(complexe *only_edges, uint32_t max_dist, uint32_t rs, uint32_t ps, uint32_t N)
{
	uint32_t d, i, pix[2], j, k, tab_numedges[3], *tab_edges[3], last_pt, start;
	uint32_t npt, *tab_vtx_to_edges, taille, nb_seg_taille, nb_seg_taille_min1, num_tot_edges; //Gives, for one vertex, all the other vertex (index+1) which share an edge with this vertex
	list *cont;
	unsigned char f;
	face_desc *h;
	complexe *result;

	if(only_edges->num_pt_obj == 0)
		complexe_compute_vertex_array(only_edges, rs, ps);

	tab_vtx_to_edges=(uint32_t*)calloc(only_edges->num_pt_obj, 2*sizeof(uint32_t));
	if(tab_vtx_to_edges==NULL)
	{
		fprintf(stderr, "cc_to_gc_only_edges(): Memory allocation error.\n");
		return(NULL);
	}

	//This will avoid code duplication
	tab_numedges[0]=only_edges->num_az;
	tab_numedges[1]=only_edges->num_ay;
	tab_numedges[2]=only_edges->num_ax;
	tab_edges[0]=only_edges->tab_az;
	tab_edges[1]=only_edges->tab_ay;
	tab_edges[2]=only_edges->tab_ax;

	//Then max_dist tells how long should be a line,
	//But we would like to avoid having a small line at the end of the curve
	//we are going to compute a mean value of the lines length
	num_tot_edges=tab_numedges[0]+tab_numedges[1]+tab_numedges[2];
	if(num_tot_edges%max_dist==0)
	{
		taille=max_dist;
		nb_seg_taille=num_tot_edges/max_dist;
		nb_seg_taille_min1=0;
	}
	else
	{
		d=num_tot_edges/max_dist +1;
		if(num_tot_edges%d==0)
		{
			taille=num_tot_edges/d;
			nb_seg_taille=d;
			nb_seg_taille_min1=0;
		}
		else
		{
			taille=num_tot_edges/d +1;
			nb_seg_taille_min1=taille*d-num_tot_edges;
			nb_seg_taille=d-nb_seg_taille_min1;
		}
	}

	assert(num_tot_edges == (taille*nb_seg_taille + (taille-1)*nb_seg_taille_min1));

	f=CC_AZ;
	for(i=0; i<3; i++)
	{
		for(j=0; j<tab_numedges[i]; j++)
		{
			cont=cca_list_subfaces_dimension_direct_under(tab_edges[i][j], f, rs, ps);
			if(cont==NULL)
			{
				fprintf(stderr, "cc_to_gc_only_simple_edges(): Error in subfunction.\n");
				return(NULL);
			}

			assert(list_getsize(cont)==2);
			for(k=0; k<2; k++)
			{
				h=(face_desc*)list_pop_pointer(cont);
				pix[k]=dichotomic_research_on_uint32t(only_edges->tab_pt_obj, h->pixnumber, only_edges->num_pt_obj);
				if(pix[k]==0)
				{
					fprintf(stderr, "cc_to_gc_only_simple_edges(): Could not find vertice in complex.\n");
					return(NULL);
				}
				if(tab_vtx_to_edges[2*(pix[k]-1)+1]!=0)
				{
					fprintf(stderr, "cc_to_gc_only_simple_edges(): Input is not a simple curve.\n");
					return(NULL);
				}
			}

			list_delete(cont, NO_FREE_DATA);

			//We found the two extremities of the edge, now fill the array
			for(k=0; k<2; k++)
			{
				if(tab_vtx_to_edges[2*(pix[k]-1)]==0)
					tab_vtx_to_edges[2*(pix[k]-1)]=pix[(k+1)%2];
				else
					tab_vtx_to_edges[2*(pix[k]-1)+1]=pix[(k+1)%2];
			}
		}
		f*=2;
	}


	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cc_to_gc_only_simple_edges(): Could not create new complex.\n");
		return(NULL);
	}
	result->TYP_COMPLEXE=GENERAL_COMPLEXE;


	//cont list will be used to scan the curve in order
	cont=list_newlist(MODE_FIFO, 100);
	if(cont==NULL)
	{
		fprintf(stderr, "cc_to_gc_only_simple_edges(): Could not create new list.\n");
		return(NULL);
	}

	start=0;
	for(i=0; i<only_edges->num_pt_obj; i++)
	{
		if(tab_vtx_to_edges[2*i+1]==0)
		{
			list_push_uint32_t(cont, i);
			start=only_edges->tab_pt_obj[i];
			break;
		}
	}

	if(list_isempty(cont))
	{
		list_push_uint32_t(cont, 0);
		start=only_edges->tab_pt_obj[0];
	}

	complexe_add_element(result, start, GC_EDGE);
	last_pt=start;
	npt=0;

	//Now, scan
	while(!list_isempty(cont))
	{
		i=list_pop_uint32_t(cont);

		if(tab_vtx_to_edges[2*i]!=0)
		{
			j=tab_vtx_to_edges[2*i];
			tab_vtx_to_edges[2*i]=0;
		}
		else
		{
			j=tab_vtx_to_edges[2*i+1];
			tab_vtx_to_edges[2*i+1]=0;
		}

		if(j!=0)
		{
			j--;
			if(tab_vtx_to_edges[2*j]==(i+1))
				tab_vtx_to_edges[2*j]=0;
			else if(tab_vtx_to_edges[2*j+1]==(i+1))
				tab_vtx_to_edges[2*j+1]=0;
			else
			{
				fprintf(stderr, "cc_to_gc_only_simple_edges(): Internal array badly built.\n");
				return(NULL);
			}
			list_push_uint32_t(cont, j);
			npt++;

			if(last_pt==start)
				last_pt=only_edges->tab_pt_obj[j];

			assert(nb_seg_taille + nb_seg_taille_min1 > 0);

			if(nb_seg_taille > 0 && npt > taille)
			{
				nb_seg_taille--;
				start=last_pt;
				complexe_add_element(result, start, GC_EDGE);
				npt=1;
			}
			else if(nb_seg_taille==0 && nb_seg_taille_min1 > 0 && npt > taille-1)
			{
				nb_seg_taille_min1--;
				start=last_pt;
				complexe_add_element(result, start, GC_EDGE);
				npt=1;
			}

			last_pt=only_edges->tab_pt_obj[j];
		}
		else
		{
			if(last_pt!=start)
			{
				if(nb_seg_taille > 0)
				{
					assert(npt==taille);
					nb_seg_taille--;
				}
				else if(nb_seg_taille_min1 > 0)
				{
					assert(npt==taille-1);
					nb_seg_taille_min1--;
				}
				complexe_add_element(result, last_pt, GC_EDGE);
			}
		}
	}

	//assert(nb_seg_taille + nb_seg_taille_min1 == 0);

	list_delete(cont, NO_FREE_DATA);
	free(tab_vtx_to_edges);
	complexe_close_element(result, GC_EDGE);
	return(result);
}



complexe *cc_to_gc_one_surface(list* gcomplexe_contour, uint32_t rs, uint32_t ps, uint32_t N)
{
	complexe *result, *cpl, **tab_complexe;
	uint32_t i, j, k, *tab_extremities, start, end, curves_drawn;


	result=complexe_new_complexe();
	if(result==NULL)
	{
		fprintf(stderr, "cc_to_gc_one_surface(): Memory allocation error for new complexe.\n");
		return(NULL);
	}
	result->TYP_COMPLEXE=GENERAL_COMPLEXE;


	tab_extremities=(uint32_t*)calloc(list_getsize(gcomplexe_contour), 2*sizeof(uint32_t));
	if(tab_extremities==NULL)
	{
		fprintf(stderr, "cc_to_gc_one_surface(): Memory allocation error.\n");
		complexe_free_complexe(result);
		return(NULL);
	}

	tab_complexe=(complexe**)calloc(list_getsize(gcomplexe_contour), sizeof(complexe*));
	if(tab_complexe==NULL)
	{
		fprintf(stderr, "cc_to_gc_one_surface(): Memory allocation error (2).\n");
		free(tab_extremities);
		complexe_free_complexe(result);
		return(NULL);
	}

	//*************************************
	//Build some useful data structures
	//*************************************
	for(i=0; i<gcomplexe_contour->size; i++)
	{
		cpl=(complexe*)list_pop_pointer(gcomplexe_contour);
		if(cpl->TYP_COMPLEXE != GENERAL_COMPLEXE || cpl->num_edges!=1)
		{
			fprintf(stderr, "cc_to_gc_one_surface(): Border edges must be list of simple curve given as general complex.\n");
			free(tab_complexe);
			free(tab_extremities);
			complexe_free_complexe(result);
			return(NULL);
		}

		tab_complexe[i]=cpl;
		tab_extremities[2*i]=cpl->tab_edges_pts[0];
		tab_extremities[2*i+1]=cpl->tab_edges_pts[cpl->tab_edges[0]-1];
		list_push_pointer(gcomplexe_contour, cpl);
	}



	//******************************************************
	//Scan the edges and look if it defines a closed contour
	//*******************************************************
	//First, check each point and check it appears only two times in the array (not more, not less)
	for(i=0; i<2*gcomplexe_contour->size; i++)
	{
		k=0;
		for(j=0; j<2*gcomplexe_contour->size; j++)
			if(tab_extremities[i] == tab_extremities[j] && i!=j)
				k++;

		if(k!=1)
		{
			fprintf(stdout, "cc_to_gc_one_surface(): Warning, surface does not have regular border\n");
			free(tab_complexe);
			free(tab_extremities);
			return(result);
		}
	}


	//Now, we know we have a regular border, let's draw the face
	start=0;
	end=0;
	curves_drawn=0;
	while(end!=start || curves_drawn==0)
	{
		if(end%2==0)
		{
			for(j=0; j<tab_complexe[end/2]->num_edges_pts -1; j++)
				complexe_add_element(result, tab_complexe[end/2]->tab_edges_pts[j], GC_FACE);
			end++;
		}
		else
		{
			for(j=tab_complexe[end/2]->num_edges_pts -1; j>=1; j--)
				complexe_add_element(result, tab_complexe[end/2]->tab_edges_pts[j], GC_FACE);
			end--;
		}

		curves_drawn++;

		//Now, look for the next curve which should be drawn
		for(j=0; j<2*gcomplexe_contour->size; j++)
		{
			if(j!=end && tab_extremities[j]==tab_extremities[end])
				break;
		}
		assert(j!=2*list_getsize(gcomplexe_contour));
		end=j;
	}
	//Add the final point and close (we do not add the final element as it is plain it should be closed)
	//complexe_add_element(result, tab_extremities[end], GC_FACE);
	complexe_close_element(result, GC_FACE);

	if(curves_drawn != list_getsize(gcomplexe_contour))
	{
		fprintf(stdout, "cc_to_gc_one_surface(): Warning, surface does not have regular border (two pieces?)\n");
		free(tab_complexe);
		free(tab_extremities);
		result->num_faces=0;
		result->num_faces_pts=0;
		return(result);
	}

	//The end
	free(tab_complexe);
	free(tab_extremities);
	return(result);
}


int32_t complexe_to_cplfile(complexe *cpl, FILE *f)
{
	if(fwrite(&(cpl->TYP_COMPLEXE), sizeof(unsigned char), 1, f) != 1)
	{
		fprintf(stderr, "complexe_to_cplfile(): Error in writing (1)\n");
		return(-1);
	}
	fflush(f);
	if(fwrite(&(cpl->num_pt_obj), sizeof(uint32_t), 1, f) != 1)
	{
		fprintf(stderr, "complexe_to_cplfile(): Error in writing (2)\n");
		return(-1);
	}
	if(fwrite(cpl->tab_pt_obj, sizeof(uint32_t), cpl->num_pt_obj, f) != cpl->num_pt_obj)
	{
		fprintf(stderr, "complexe_to_cplfile(): Error in writing (3)\n");
		return(-1);
	}
	fflush(f);

	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			if(fwrite(&(cpl->num_ax), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (4)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_ax, sizeof(uint32_t), cpl->num_ax, f) != cpl->num_ax)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (5)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_ay), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (6)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_ay, sizeof(uint32_t), cpl->num_ay, f) != cpl->num_ay)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (7)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_az), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (8)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_az, sizeof(uint32_t), cpl->num_az, f) != cpl->num_az)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (9)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fxy), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (10)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_fxy, sizeof(uint32_t), cpl->num_fxy, f) != cpl->num_fxy)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (11)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fxz), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (12)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_fxz, sizeof(uint32_t), cpl->num_fxz, f) != cpl->num_fxz)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (13)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fyz), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (14)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_fyz, sizeof(uint32_t), cpl->num_fyz, f) != cpl->num_fyz)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (15)\n");
				return(-1);
			}
			fflush(f);
			break;

		case(GENERAL_COMPLEXE):
			if(fwrite(&(cpl->num_edges_pts), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (16)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_edges_pts, sizeof(uint32_t), cpl->num_edges_pts, f) != cpl->num_edges_pts)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (17)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_edges), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (18)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_edges, sizeof(uint32_t), cpl->num_edges, f) != cpl->num_edges)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (19)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_faces_pts), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (20)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_faces_pts, sizeof(uint32_t), cpl->num_faces_pts, f) != cpl->num_faces_pts)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (21)\n");
				return(-1);
			}
			fflush(f);
			if(fwrite(&(cpl->num_faces), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (22)\n");
				return(-1);
			}
			if(fwrite(cpl->tab_faces, sizeof(uint32_t), cpl->num_faces, f) != cpl->num_faces)
			{
				fprintf(stderr, "complexe_to_cplfile(): Error in writing (23)\n");
				return(-1);
			}
			fflush(f);
			break;
	}

	return(0);
}



complexe* cplfile_to_complexe(FILE *f)
{
	complexe *cpl = complexe_new_complexe();
	if(cpl==NULL)
	{
		fprintf(stderr, "cplfile_to_complexe(): Memory allocation error.\n");
		return(NULL);
	}


	if(fwrite(&(cpl->TYP_COMPLEXE), sizeof(unsigned char), 1, f) != 1)
	{
		fprintf(stderr, "complexe_to_file(): Error in writing (1)\n");
		return(NULL);
	}
	fflush(f);
	if(fwrite(&(cpl->num_pt_obj), sizeof(uint32_t), 1, f) != 1)
	{
		fprintf(stderr, "complexe_to_file(): Error in writing (2)\n");
		return(NULL);
	}
	if(fwrite(cpl->tab_pt_obj, sizeof(uint32_t), cpl->num_pt_obj, f) != cpl->num_pt_obj)
	{
		fprintf(stderr, "complexe_to_file(): Error in writing (3)\n");
		return(NULL);
	}
	fflush(f);

	switch(cpl->TYP_COMPLEXE)
	{
		case(CCA_COMPLEXE):
			if(fwrite(&(cpl->num_ax), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (4)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_ax, sizeof(uint32_t), cpl->num_ax, f) != cpl->num_ax)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (5)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_ay), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (6)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_ay, sizeof(uint32_t), cpl->num_ay, f) != cpl->num_ay)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (7)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_az), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (8)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_az, sizeof(uint32_t), cpl->num_az, f) != cpl->num_az)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (9)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fxy), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (10)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_fxy, sizeof(uint32_t), cpl->num_fxy, f) != cpl->num_fxy)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (11)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fxz), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (12)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_fxz, sizeof(uint32_t), cpl->num_fxz, f) != cpl->num_fxz)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (13)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_fyz), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (14)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_fyz, sizeof(uint32_t), cpl->num_fyz, f) != cpl->num_fyz)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (15)\n");
				return(NULL);
			}
			fflush(f);
			break;

		case(GENERAL_COMPLEXE):
			if(fwrite(&(cpl->num_edges_pts), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (16)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_edges_pts, sizeof(uint32_t), cpl->num_edges_pts, f) != cpl->num_edges_pts)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (17)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_edges), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (18)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_edges, sizeof(uint32_t), cpl->num_edges, f) != cpl->num_edges)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (19)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_faces_pts), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (20)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_faces_pts, sizeof(uint32_t), cpl->num_faces_pts, f) != cpl->num_faces_pts)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (21)\n");
				return(NULL);
			}
			fflush(f);
			if(fwrite(&(cpl->num_faces), sizeof(uint32_t), 1, f) != 1)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (22)\n");
				return(NULL);
			}
			if(fwrite(cpl->tab_faces, sizeof(uint32_t), cpl->num_faces, f) != cpl->num_faces)
			{
				fprintf(stderr, "complexe_to_file(): Error in writing (23)\n");
				return(NULL);
			}
			fflush(f);
			break;
	}

	return(0);
}


