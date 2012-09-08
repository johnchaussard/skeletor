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

#define USAGE "<input_surfaxis_cc> <original_cc_image> <output_surfaxis_cc>"


int32_t main(int argc, char *argv[])
{
	uint32_t rs, cs, d, ps, N, i, x, y, z, r;
	struct xvimage *image, *original;

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
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}

	original=readimage(argv[2]);
	if (original==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[2]);
		return(-1);
	}
	else if(datatype(original)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
		return(-1);
	}

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;
	N=ps*d;


	//Then, remove the edges which are not free...
	i=0;
	for(z=0; z<d; z++)
		for(y=0; y<cs; y++)
			for(x=0; x<rs; x++)
			{
				if(UCHARDATA(image)[i] != 0)
				{
					if( (UCHARDATA(image)[i] & CC_AX) !=0)
					{
						//Remove the edge if it is not free...
						r=cca_cardinal_containers(image, i, x, y, z, CC_AX, rs, ps);
						if(r<1 || r>2)
							UCHARDATA(image)[i]=UCHARDATA(image)[i]-CC_AX;
					}

					if( (UCHARDATA(image)[i] & CC_AY) !=0)
					{
						r=cca_cardinal_containers(image, i, x, y, z, CC_AY, rs, ps);
						if(r<1 || r>2)
							UCHARDATA(image)[i]=UCHARDATA(image)[i]-CC_AY;
					}

					if( (UCHARDATA(image)[i] & CC_AZ) !=0)
					{
						r=cca_cardinal_containers(image, i, x, y, z, CC_AZ, rs, ps);
						if(r<1 || r>2)
							UCHARDATA(image)[i]=UCHARDATA(image)[i]-CC_AZ;
					}
				}
				i++;
			}

	//Finally, keep only the faces which still contain an edge...
	i=0;
	for(z=0; z<d; z++)
		for(y=0; y<cs; y++)
			for(x=0; x<rs; x++)
			{
				UCHARDATA(image)[i]&=(255-CC_VOL-CC_PT); //In case...

				if ((UCHARDATA(image)[i]&CC_FXY)!=0)
				{
					if( (UCHARDATA(image)[i]&CC_AX)==0 || (UCHARDATA(image)[i]&CC_AY)==0 || (UCHARDATA(image)[i+rs]&CC_AX)==0 || (UCHARDATA(image)[i+1]&CC_AY)==0)
					{
						//UCHARDATA(image)[i]&=255-CC_FXY;
						if(z<d-1 && (UCHARDATA(original)[i]&CC_VOL)!=0) UCHARDATA(image)[i]|=CC_VOL;
						if(z>0 && (UCHARDATA(original)[i-ps]&CC_VOL)!=0) UCHARDATA(image)[i-ps]|=CC_VOL;
					}
				}


				if ((UCHARDATA(image)[i]&CC_FXZ)!=0)
				{
					if( (UCHARDATA(image)[i]&CC_AX)==0 || (UCHARDATA(image)[i]&CC_AZ)==0 || (UCHARDATA(image)[i+ps]&CC_AX)==0 || (UCHARDATA(image)[i+1]&CC_AZ)==0)
					{
						//UCHARDATA(image)[i]&=255-CC_FXZ;
						if(y<cs-1 && (UCHARDATA(original)[i]&CC_VOL)!=0) UCHARDATA(image)[i]|=CC_VOL;
						if(y>0 && (UCHARDATA(original)[i-rs]&CC_VOL)!=0) UCHARDATA(image)[i-rs]|=CC_VOL;
					}
				}

				if ((UCHARDATA(image)[i]&CC_FYZ)!=0)
				{
					if( (UCHARDATA(image)[i]&CC_AZ)==0 || (UCHARDATA(image)[i]&CC_AY)==0 || (UCHARDATA(image)[i+rs]&CC_AZ)==0 || (UCHARDATA(image)[i+ps]&CC_AY)==0)
					{
						//UCHARDATA(image)[i]&=255-CC_FYZ;
						if(x<rs-1 && (UCHARDATA(original)[i]&CC_VOL)!=0) UCHARDATA(image)[i]|=CC_VOL;
						if(x>0 && (UCHARDATA(original)[i-1]&CC_VOL)!=0) UCHARDATA(image)[i-1]|=CC_VOL;
					}
				}

				UCHARDATA(image)[i]&=255-CC_AX-CC_AY-CC_AZ;
				i++;
			}



	cca_makecomplex(image);

	//Finished...
	writeimage(image, argv[3]);
	freeimage(image);

	return(0);
}


