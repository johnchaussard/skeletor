/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libinventor.h>
#include <libcomplex.h>
#include <liblist.h>

#define USAGE "<input_cca> <input_intersection_edges_cca> <output_cca>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *edges, *debug;
	list *ss_result, *ss_keep, *container;
	complexe *temp, *intersect_edges;
	uint32_t i, rs, ps, cs, d, cpt, keep, face, j, num_intersect_vertex;
	face_desc *fd;



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
		freeimage(image);
		return(-1);
	}

	//We read input image
	edges=readimage(argv[2]);
	if (edges==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(-1);
	}
	else if(datatype(edges)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		freeimage(image);
		freeimage(edges);
		return(-1);
	}

	debug=allocimage(NULL, rowsize(edges), colsize(edges), depth(edges), VFF_TYP_1_BYTE);
	for(i=0; i<(uint32_t)(rowsize(debug)*colsize(debug)*depth(debug)); i++)
	{
		UCHARDATA(debug)[i]=0;
	}

	rs=rowsize(edges);
	cs=colsize(edges);
	d=depth(edges);
	ps=rs*cs;



	//*******************************************************
	//Preparation of the image and the scene
	//*******************************************************

	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);



	//********************************************************
	//Make surface segmentation
	//********************************************************

	//Get the intersection edges
	intersect_edges=cca_to_complexe(edges);
	if(intersect_edges==NULL)
	{
		fprintf(stderr, "Error in function cca_to_complexe()\n");
		freeimage(edges);
		freeimage(image);
		return -1;
	}

	ss_result=cca_simple_surface_segmentation_with_intersection_edges(image, intersect_edges);
	if(ss_result==NULL)
	{
		fprintf(stderr, "Error: cca_simple_surface_segmentation() failed.\n");
		freeimage(image);
		freeimage(edges);
		return(-1);
	}
	//We don't need intersection edges, which are in the ss_result structure
	intersect_edges=NULL;
	//And neither the original image
	freeimage(image);

	//We don't care the two first items of the list as they are vertex and edges...
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);
	temp=(complexe*)list_pop_pointer(ss_result);
	complexe_free_complexe(temp);




	//****************************************************
	//Scan each surface and output only those with regular border
	//****************************************************

	cpt=0;
	j=0;
	ss_keep=list_newlist(MODE_FIFO, 64);   //The list where the surfaces to keep will be put.
	if(ss_keep==NULL)
	{
		fprintf(stderr, "Error in list_newlist function.\n");
		return(-1);
	}


	//For each surface
	while(!list_isempty(ss_result))
	{
		temp=(complexe*)list_pop_pointer(ss_result);

		//Delete, in case, points, edges and volumes from surface (should not be there anyway)
		complexe_remove_faces(temp, CC_PT);
		complexe_remove_faces(temp, CC_AX);
		complexe_remove_faces(temp, CC_AY);
		complexe_remove_faces(temp, CC_AZ);
		complexe_remove_faces(temp, CC_VOL);

		//Draw the complex in the edge image
		complexe_add_to_cca(edges, temp);

		//Compute the vertices of the complex
		complexe_compute_vertex_array(temp, rs, ps);

		//Scan each point of the complex, and look for points which are in contact with thre or more intersection edges
		keep=1;
		num_intersect_vertex=0;
		for(i=0; i<temp->num_pt_obj && keep==1; i++)
		{
			//If current point is contained in edge intersection images and is in three or more edges
			if(	(UCHARDATA(edges)[temp->tab_pt_obj[i]] & CC_PT)!=0 &&
				(cca_cardinal_containers(edges, temp->tab_pt_obj[i], getxfrompixnum(temp->tab_pt_obj[i],rs,ps), getyfrompixnum(temp->tab_pt_obj[i],rs,ps), getzfrompixnum(temp->tab_pt_obj[i],rs,ps), CC_PT, rs, ps)>2))
			{
				UCHARDATA(debug)[temp->tab_pt_obj[i]]=1;
				num_intersect_vertex++;
				//Look all edges containing the point
				container=cca_list_container(edges, temp->tab_pt_obj[i], getxfrompixnum(temp->tab_pt_obj[i],rs,ps), getyfrompixnum(temp->tab_pt_obj[i],rs,ps), getzfrompixnum(temp->tab_pt_obj[i],rs,ps), CC_PT, rs, ps);
				if(container==NULL)
				{
					fprintf(stderr, "Error in function cca_list_container()\n");
					freeimage(edges);
					return(-1);
				}

				//How many edge belong to current surface ?
				face=0;
				while(!list_isempty(container))
				{
					fd=(face_desc*)list_pop_pointer(container);
					if(cca_cardinal_containers(edges, fd->pixnumber, getxfrompixnum(fd->pixnumber,rs,ps), getyfrompixnum(fd->pixnumber,rs,ps), getzfrompixnum(fd->pixnumber,rs,ps), fd->face_type, rs, ps)>0)
						face++;
					free(fd);
				}
				list_delete(container, NO_FREE_DATA);

				//More than three edges around current point belong to current surface, don't keep current surface
				if(face>2)
					keep=0;
			}
		}

		//Remove the complex from the image
		complexe_remove_faces(temp, CC_PT);
		complexe_remove_from_cca(edges, temp);

		if(num_intersect_vertex < 3)
			keep=0;

		//If we keep the complex, transfer it to another list, else, remove it.
		if(keep==1)
		{
			list_push_pointer(ss_keep, temp);
			if(num_intersect_vertex < 3)
			{
				fprintf(stdout, "Surface num %d with %d elements has not enough vertex.\n", j, temp->num_fxy + temp->num_fyz + temp->num_fxz);
			}
		}
		else
		{
			fprintf(stdout, "Removed surface %d (%d) (problem around point (%d, %d, %d))\n", j, temp->num_fxy + temp->num_fyz + temp->num_fxz, getxfrompixnum(temp->tab_pt_obj[i], rs, ps), getyfrompixnum(temp->tab_pt_obj[i], rs, ps), getzfrompixnum(temp->tab_pt_obj[i], rs, ps));
			complexe_free_complexe(temp);
			cpt++;
		}
		j++;
	}

	list_delete(ss_result, NO_FREE_DATA);
	freeimage(edges);


	//***********************************************
	//Print kept faces to output image
	//***********************************************

	fprintf(stdout, "%d surfaces removed.\n", cpt);

	//Output image for result
	image=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
	if(image==NULL)
	{
		fprintf(stderr, "Could not allocate memory for output image.\n");
		return(-1);
	}

	while(!list_isempty(ss_keep))
	{
		temp=(complexe*)list_pop_pointer(ss_keep);
		complexe_add_to_cca(image, temp);
		complexe_free_complexe(temp);
	}

	j=0;
	for(i=0; i<(uint32_t)(rowsize(debug)*colsize(debug)*depth(debug)); i++)
	{
		if(UCHARDATA(debug)[i]==1)
		{
			j++;
		}
	}

	fprintf(stdout, "%d\n", j);


	//**************************************
	//Finish
	//*************************************

	list_delete(ss_keep, NO_FREE_DATA);
	cca_makecomplex(image);
	writeimage(image, argv[3]);
	freeimage(image);

	return(0);
}



