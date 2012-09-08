//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mccodimage.h>

//Librairie perso
#include <libeps.h>


eps_scene* eps_new_scene(FILE *output_file, int zoom, int max_x, int max_y)
{
	eps_scene *result;

	result=(eps_scene*)calloc(1, sizeof(eps_scene));
	if(result==NULL)
	{
		fprintf(stderr, "eps_new_scene(): Memory allocation error (1).\n");
		return(NULL);
	}
	result->output=output_file;
	result->zoom=zoom;

	result->do_not_access_tx=0;
	result->do_not_access_ty=0;

	result->width=max_x;
	result->height=max_y;

	return(result);
}


void eps_delete_scene(eps_scene* scene)
{
	free(scene);
}


void eps_init_scene(eps_scene* scene)
{
	float width=(float)scene->zoom/22.0;
	fprintf(scene->output, "%%!PS-Adobe-3.0 EPSF-3.0\n%%%%Pages: 1\n%%%%BoundingBox: -1 -1 %d %d\n%%%%DocumentData: Clean7Bit\n%%%%LanguageLevel: 2\n%%%%Page: 1 1\n", (scene->width+1)*scene->zoom, (scene->height+1)*scene->zoom);
	fprintf(scene->output, "/basicsquare {\nnewpath\n0 0 moveto\n0 %d rlineto\n%d 0 rlineto\n0 -%d rlineto\nclosepath\nsetrgbcolor\nfill\n} def\n", scene->zoom, scene->zoom, scene->zoom);
	fprintf(scene->output, "/basichline {\nnewpath\n0 %d moveto\n%d 0 rlineto\nsetrgbcolor\n%f setlinewidth\nstroke\n} def\n", scene->zoom, scene->zoom, width);
	fprintf(scene->output, "/basicvline {\nnewpath\n0 0 moveto\n0 %d rlineto\nsetrgbcolor\n%f setlinewidth\nstroke\n} def\n", scene->zoom, width);
	fprintf(scene->output, "/basicpoint {\n0 %d %f 0 360 arc closepath\nsetrgbcolor\nfill\n} def\n", scene->zoom, 3*width);
}


void eps_close_scene(eps_scene* scene)
{
	fprintf(scene->output, "\%\% EOF\n");
	fflush(scene->output);
}


void eps_define_square(eps_scene* scene, float r, float g, float b, char *name)
{
	fprintf(scene->output,"/%s {\n%3f %3f %3f basicsquare\n} def\n", name, r, g, b);
}


void eps_define_hline(eps_scene* scene, float r, float g, float b, char *name)
{
	fprintf(scene->output,"/%s {\n%3f %3f %3f basichline\n} def\n", name, r, g, b);
}


void eps_define_vline(eps_scene* scene, float r, float g, float b, char *name)
{
	fprintf(scene->output,"/%s {\n%3f %3f %3f basicvline\n} def\n", name, r, g, b);
}


void eps_define_point(eps_scene* scene, float r, float g, float b, char *name)
{
	fprintf(scene->output,"/%s {\n%3f %3f %3f basicpoint\n} def\n", name, r, g, b);
}


void eps_draw_element(eps_scene* scene, char* name, int posx, int posy)
{
	fprintf(scene->output,"%d %d translate\n%s\n", (posx-scene->do_not_access_tx)*scene->zoom, ((scene->height-posy)-scene->do_not_access_ty)*scene->zoom, name);
	scene->do_not_access_tx=posx;
	scene->do_not_access_ty=scene->height-posy;
}



