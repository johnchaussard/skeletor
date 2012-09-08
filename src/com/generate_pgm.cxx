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


#define USAGE "<xdim> <ydim> <zdim> <byte|short|int|float|double> <out.pgm>"

int main(int argc, char* argv[])
{
	int32_t sizex, sizey, sizez;
	struct xvimage* output;

	if(argc!=6)
	{
		fprintf(stderr, "%s %s\n", argv[0], USAGE);
		return(1);
	}

	sizex=atoi(argv[1]);
	if(sizex<=0) fprintf(stderr, "X size must be positive.\n");

	sizey=atoi(argv[2]);
	if(sizey<=0) fprintf(stderr, "Y size must be positive.\n");

	sizez=atoi(argv[3]);
	if(sizez<=0) fprintf(stderr, "Z size must be positive.\n");

	if(strcmp(argv[4], "byte")==0)
		output=allocimage(NULL, sizex, sizey, sizez, VFF_TYP_1_BYTE);
	else if(strcmp(argv[4], "short")==0)
		output=allocimage(NULL, sizex, sizey, sizez, VFF_TYP_2_BYTE);
	else if(strcmp(argv[4], "int")==0)
		output=allocimage(NULL, sizex, sizey, sizez, VFF_TYP_4_BYTE);
	else if(strcmp(argv[4], "float")==0)
		output=allocimage(NULL, sizex, sizey, sizez, VFF_TYP_FLOAT);
	else if(strcmp(argv[4], "double")==0)
		output=allocimage(NULL, sizex, sizey, sizez, VFF_TYP_DOUBLE);
	else
	{
		fprintf(stdout, "Image type can be byte, short, int, long or float (you gave %s)\n", argv[4]);
		return(1);
	}

	if(output==NULL)
	{
		fprintf(stderr, "Memory allocation error.\n");
		return(1);
	}

	writeimage(output, argv[5]);
	freeimage(output);
	return(0);
}
