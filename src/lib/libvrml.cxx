//Librairies standardes
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

//Patch des types
#include <patch_types.h>

//Librairie perso
#include <libvrml.h>


vrml_scene* vrml_new_scene(char *output_file)
{
	vrml_scene *result;

	result=(iv_scene*)calloc(1, sizeof(iv_scene));
	if(result==NULL)
	{
		fprintf(stderr, "vrml_new_scene(): Memory allocation error (1).\n");
		return(NULL);
	}
	result->num_tabulations=0;

	result->output=fopen(output_file, "wb");
	if(result->output == NULL)
	{
		fprintf(stderr, "vrml_new_scene(): Could not create file %s.\n", output_file);
		return(NULL);
	}

	fprintf(result->output, "#VRML V1.0 ascii\n\n");
	fprintf(result->output, "ShapeHints {\n\tvertexOrdering COUNTERCLOCKWISE\n}\n");

	return(result);
}







