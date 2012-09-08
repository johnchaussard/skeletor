//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <assert.h>



#include <mcimage.h>
#include <mccodimage.h>

#include <libcrop.h>
#include <libcomplex.h>


int main(int argc, char *argv[])
{
	complexe *compedge, *cpl, *cpl2, **tab_surfaces, **tab_edges;
	struct xvimage *edges, *imsurfaces;
	uint32_t i, N, rs, ps, k;
	list *l, *surfaces, **surfaces_to_curve, *contour;
	iv_scene *scene;
	char name[100];
	FILE *output;

	edges=readimage(argv[1]);
	imsurfaces=readimage(argv[2]);
	compedge=cca_to_complexe(edges);
	assert(edges!=NULL);

	rs=rowsize(edges);
	ps=colsize(edges)*rs;
	N=ps*depth(edges);

	cpl=complexe_new_complexe();

	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(edges)[i] & CC_PT) != 0)
		{
			if(cca_cardinal_containers(edges, i, getxfrompixnum(i, rs, ps), getyfrompixnum(i, rs, ps), getzfrompixnum(i, rs, ps), CC_PT, rs, ps)!=2)
			{
				complexe_add_element(cpl, i, CC_PT);
			}
		}
	}

	surfaces=cca_simple_surface_segmentation_with_intersection_edges_and_exclusion_inclusion_image(imsurfaces, compedge, NULL, KEEP);

	l=cca_simple_curve_decomposition(edges, cpl);


	//In case we don't have a complex, uncomment following
	//cca_makecomplex(image);

	scene=inventor_new_scene(10, NULL);

	//We add to our object the main materials (keep the surfaces for later)
	inventor_add_material_to_scene(scene, "MatPoint", POS_PT, 0.0, 0.0, 0.0, 0.1, 0.4, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAX", POS_AX, 0.0, 0.0, 0.0, 0.1, 0.1, 0.6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAY", POS_AY, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatAZ", POS_AZ, 0.0, 0.0, 0.0, 0.1, 0.1, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVXY", POS_VXY, 0.0, 0.0, 0.0, 0.65, 0.65, 0.65, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVXZ", POS_VXZ, 0.0, 0.0, 0.0, 0.50, 0.50, 0.50, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatVYZ", POS_VYZ, 0.0, 0.0, 0.0, 0.80, 0.80, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatFXY", POS_FXY, 0.65, 0.0, 0.0, 0.65, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatFXZ", POS_FXZ, 0.50, 0.0, 0.0, 0.50, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	inventor_add_material_to_scene(scene, "MatFYZ", POS_FYZ, 0.80, 0.0, 0.0, 0.80, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


	i=0;

	tab_surfaces=(complexe**)calloc(surfaces->cpt, sizeof(complexe*));
	tab_edges=(complexe**)calloc(l->cpt, sizeof(complexe*));

	for(i=0; i<surfaces->cpt; i++)
	{
		cpl=(complexe*)list_pop(surfaces);
		tab_surfaces[i]=cpl;
		list_push(surfaces, (void*)cpl);
	}
	for(i=0; i<l->cpt; i++)
	{
		cpl=(complexe*)list_pop(l);
		tab_edges[i]=cpl;
		list_push(l, (void*)cpl);
	}

	surfaces_to_curve=cca_link_surfaces_to_border_edges(surfaces, l, rs, ps, N);

	for(i=0; i<l->cpt; i++)
	{
		complexe_compute_vertex_array(tab_edges[i], rs, ps);
		cpl=cc_to_gc_only_simple_edges(tab_edges[i],16, rs, ps, N);
		complexe_free_complexe(tab_edges[i]);
		tab_edges[i]=cpl;
	}

	list_delete(l, NO_FREE_DATA);
	contour=list_newlist(MODE_FIFO);

	i=0;
	for(i=0; i<surfaces->cpt; i++)
	{
		sprintf(name, "out/surf%d.iv", i);
		output = fopen(name, "wb");
		scene->output=output;
		inventor_init_file(scene);

		inventor_new_object(scene);
		inventor_set_drawstyle(scene, FILLED, 1, 1);

		while(!list_isempty(surfaces_to_curve[i]))
		{
			k=(uint32_t)list_pop(surfaces_to_curve[i]);
			cpl=tab_edges[k];
			list_push(contour, (void*)cpl);
		}

		cpl2=cc_to_gc_one_surface(contour, rs, ps, N);
		complexe_compute_vertex_array(cpl2, rs, ps);
		complexe_to_inventor(scene, cpl2, 0, NULL, rs, ps, 0);
		inventor_close_object(scene);
		complexe_free_complexe(cpl2);
		fflush(scene->output);
		fclose(scene->output);
	}
	return(0);
}


