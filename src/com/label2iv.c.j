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
#include <libcomplex.h>

#define USAGE "<pgm_image> <output_inventor>"



int32_t main(int argc, char *argv[])
{
	struct xvimage *image;
	uint32_t N, rs, ps, i, j, max, min, length, rs_cc, ps_cc, N_cc, x, y, z, val, cs, d, num_cpl, num_pt, *tab_pt;
	complexe **list_complex;
	iv_scene *scene;
	FILE *output;
	char name[300]; //We are Spartaaaaaa !
	double r, v, b;
	complexe *temp;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=3)
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

	output = fopen(argv[2], "wb");
	if(output==NULL)
	{
		fprintf(stderr, "Error: could not create output file %s (directory exists?).\n", name);
		freeimage(image);
		return(-1);
	}



	//*****************************************
	//Get some info about the image
	//*****************************************
	rs=rowsize(image);
	rs_cc=rs+1;
	cs=colsize(image);
	d=depth(image);
	ps=cs*rs;
	ps_cc=(cs+1)*rs_cc;
	N=d*ps;
	N_cc=(d+1)*ps_cc;


      if(datatype(image) == VFF_TYP_1_BYTE)
      {
	max=UCHARDATA(image)[0];
	min=UCHARDATA(image)[0];
	for(i=1; i<N; i++)
	{
		if(UCHARDATA(image)[i] > max)
			max = UCHARDATA(image)[i];

		if(UCHARDATA(image)[i] < min)
			min = UCHARDATA(image)[i]; //The min value won't be represented (background)
	}

	//Create a list of complex, each complex will contain the faces having a given label
	length=max-min;
	list_complex = (complexe**) calloc(length, sizeof(complexe*));
	if(list_complex==NULL)	return(-1);

	//Scan the image, and for each grey level value, add faces to the complex	num_cpl=0;
	for(i=0; i<N; i++)
	{
		if(UCHARDATA(image)[i] != min)
		{
			x=getxfrompixnum(i, rs, ps);
			y=getyfrompixnum(i, rs, ps);
			z=getzfrompixnum(i, rs, ps);
			val=UCHARDATA(image)[i] - min - 1;

			if(list_complex[val]==NULL)
			{
				list_complex[val] = complexe_new_complexe();
				if(list_complex[val] == NULL)
					return(-1);
				num_cpl++;
			}

			//We add a face to the complex only if the neighbouring cube which contains this face does not have same label,
			//or if it has, we add the face if it was not already added

			//Do we add CC_FXY top ?
			if(z==0 || val!=UCHARDATA(image)[i-ps]-min-1)
				complexe_add_element(list_complex[val], x+y*rs_cc+z*ps_cc, CC_FXY);

			//Do we add CC_FXZ top ?
			if(y==0 || val!=UCHARDATA(image)[i-rs]-min-1)
				complexe_add_element(list_complex[val], x+y*rs_cc+z*ps_cc, CC_FXZ);

			//Do we add CC_FXZ top ?
			if(x==0 || val!=UCHARDATA(image)[i-1]-min-1)
				complexe_add_element(list_complex[val], x+y*rs_cc+z*ps_cc, CC_FYZ);

			if(z==d-1 || val!=UCHARDATA(image)[i+ps]-min-1)
				complexe_add_element(list_complex[val], x+y*rs_cc+(z+1)*ps_cc, CC_FXY);

			if(y==cs-1 || val!=UCHARDATA(image)[i+rs]-min-1)
				complexe_add_element(list_complex[val], x+(y+1)*rs_cc+z*ps_cc, CC_FXZ);

			if(x==rs-1 || val!=UCHARDATA(image)[i+1]-min-1)
				complexe_add_element(list_complex[val], x+1+y*rs_cc+z*ps_cc, CC_FYZ);
		}
	}
      }
      
      else
      {
	fprintf(stderr, "Error: Bad image type (must be byte or long).\n");
	return(-1);
      }

	freeimage(image);


	//**************************************************************
	//Send each of the complex to an output inventor file
	//**************************************************************

	//Create the scene structure
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


	scene->output=output;
	inventor_init_file(scene);

	srandom(time(NULL));

	//Create all random colors, and send them to output file
	for(i=0; i<num_cpl; i++)
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


	num_cpl=0;
	for(i=0; i<length; i++)
	{
		if(list_complex[i]!=NULL)
		{
			temp=list_complex[i];

			complexe_compute_vertex_array(temp, rs_cc, ps_cc);

			//Create new object in the inventor file
			inventor_new_object(scene);
			inventor_declare_points(scene, NULL, temp->tab_pt_obj, temp->num_pt_obj, rs_cc, ps_cc, 1);
			tab_pt=temp->tab_pt_obj;
			num_pt=temp->num_pt_obj;
			temp->num_pt_obj=0;
			temp->tab_pt_obj=NULL;

			sprintf(name, "SurfXY_%d", num_cpl);
			inventor_add_material_to_scene(scene, name, POS_FXY, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			sprintf(name, "SurfXZ_%d", num_cpl);
			inventor_add_material_to_scene(scene, name, POS_FXZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			sprintf(name, "SurfYZ_%d", num_cpl);
			inventor_add_material_to_scene(scene, name, POS_FYZ, r, v, b, r, v, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

			complexe_to_inventor(scene, temp, num_pt, tab_pt, rs_cc, ps_cc, 1);

			inventor_close_object(scene);

			fflush(scene->output);

			free(tab_pt);
			num_cpl++;
			complexe_free_complexe(temp);
		}
	}

	fclose(scene->output);
	inventor_delete_scene(scene);
	free(list_complex);


	return(0);
}












