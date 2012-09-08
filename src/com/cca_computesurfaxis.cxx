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
#include <libcomplex.h>

#define USAGE "<input_cc> <output_cc> <distmap> <surf|line> (<inhibit>)"

#define SURFACE 0
#define LINE 1


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *distmap, *inhibit, *output;
	int32_t times;
	uint32_t i, j, k, l, d, rs, cs, ps, pix, max, mode;
	complexe *cpl;
	char filename[100];


	cpl=complexe_new_complexe();



	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=5 && argc!=6)
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
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;

	if(strcmp(argv[4], "line")==0)
		mode=LINE;
	else if(strcmp(argv[4], "surf")==0)
		mode=SURFACE;
	else
	{
		fprintf(stderr, "Error: wrong mode given (%s), should give \"line\" or \"surface\"\n", argv[4]);
		return(-1);
	}


	if(argc==6)
		inhibit=readimage(argv[5]);
	else
		inhibit=allocimage(NULL, rowsize(image), colsize(image), depth(image), VFF_TYP_1_BYTE);
	//memset(&(inhibit->image_data[0]), (unsigned char)(CC_FXY|CC_FYZ|CC_FXZ), rowsize(image)*colsize(image)*depth(image));

	distmap=readimage(argv[3]);
	if(distmap==NULL)
		fprintf(stderr, "Bouh\n");

	output=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);

	max=ULONGDATA(distmap)[0];
	for(i=1; i<(rs-1)*(cs-1)*(d-1); i++)
	{
		if(ULONGDATA(distmap)[i]>max)
			max=ULONGDATA(distmap)[i];
	}
	fprintf(stdout, "max:%d\n", max);
	times=max+1;

	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//We collapse
	l=0;
	while(cca_collapse_method_facesandsubfaces2(image, output, 1)>0)
	{
		l++;
		pix=0;
		for(k=0; k<d; k++)
			for(j=0; j<cs; j++)
				for(i=0; i<rs; i++)
				{
					if(UCHARDATA(image)[pix]!=0 && ((UCHARDATA(image)[pix]&CC_VOL) == 0))
					{
						switch(mode)
						{
							case(SURFACE):
								if(((UCHARDATA(image)[pix]&CC_FXY)!=0) && (k==0 || (k>0 && ((UCHARDATA(image)[pix-ps]&CC_VOL) == 0))) )
								if(cca_getfacedist(image, distmap, i, j, k, CC_FXY, rs, ps)  < l)
								{
									UCHARDATA(output)[pix]|=CC_FXY;
									max=l;
								}

								if(((UCHARDATA(image)[pix]&CC_FXZ)!=0) && (j==0 || (j>0 && ((UCHARDATA(image)[pix-rs]&CC_VOL) == 0))) )
								if(cca_getfacedist(image, distmap, i, j, k, CC_FXZ, rs, ps)  < l)
								{
									UCHARDATA(output)[pix]|=CC_FXZ;
									max=l;
								}

								if(((UCHARDATA(image)[pix]&CC_FYZ)!=0) && (i==0 || (i>0 && ((UCHARDATA(image)[pix-1]&CC_VOL) == 0))) )
								if(cca_getfacedist(image, distmap, i, j, k, CC_FYZ, rs, ps)  < l)
								{
									UCHARDATA(output)[pix]|=CC_FYZ;
									max=l;
								}
								//break;



							case(LINE):
								if(((UCHARDATA(image)[pix]&CC_AX)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AX, rs, ps)==0 )
								if(cca_getfacedist(image, distmap, i, j, k, CC_AX, rs, ps)  < l)
								{
									cca_add_complexface(output, CC_AX, pix, rs, ps);
									max=l;
								}

								if(((UCHARDATA(image)[pix]&CC_AY)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AY, rs, ps)==0 )
								if(cca_getfacedist(image, distmap, i, j, k, CC_AY, rs, ps)  < l)
								{
									cca_add_complexface(output, CC_AY, pix, rs, ps);
									max=l;
								}

								if(((UCHARDATA(image)[pix]&CC_AZ)!=0) && cca_count_face_container(image, pix, i, j, k, CC_AZ, rs, ps)==0 )
								if(cca_getfacedist(image, distmap, i, j, k, CC_AZ, rs, ps)  < l)
								{
									cca_add_complexface(output, CC_AZ, pix, rs, ps);
									max=l;
								}
								break;

							default:
								fprintf(stderr, "Error: Unknown mode.\n");
								break;
						}


					}

					pix++;
				}



		sprintf(&filename[0], "debug_object_%d.cc", l);
		writeimage(image, filename);


		cca_makecomplex(output);
	}

	fprintf(stdout, "%d\n", max);




	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);
	freeimage(inhibit);
	freeimage(distmap);
	freeimage(output);


	return(0);
}


