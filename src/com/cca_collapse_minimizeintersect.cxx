/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Patch des types
#include <patch_types.h>

/*Les bibliotheques de PINK*/
#include <mcimage.h>
#include <mccodimage.h>

//Bibliotheque perso
#include <libcomplex.h>

#define USAGE "<input_cc> <output_cc> <surfaxis_cc|NULL> <inhibit_cc|NULL>"


int32_t main(int argc, char *argv[])
{
	uint32_t i, rs, ps, N, cs, d, x, y, z, r;
	struct xvimage *image, *surfaxis, *inhibit;

	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=5)
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


	//We read surfaxis image if one was given
	surfaxis=NULL;
	if(strcmp(argv[3], "NULL")!=0)
	{
		surfaxis=readimage(argv[3]);
		if (surfaxis==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[3]);
			return(-1);
		}
		else if(datatype(surfaxis)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
			return(-1);
		}
	}

	//We read surfaxis image if one was given
	inhibit=NULL;
	if(strcmp(argv[4], "NULL")!=0)
	{
		inhibit=readimage(argv[4]);
		if (inhibit==NULL)
		{
			fprintf(stderr, "Error: Could not read %s.\n", argv[4]);
			return(-1);
		}
		else if(datatype(inhibit)!=VFF_TYP_1_BYTE)
		{
			fprintf(stderr, "Error: only 1 byte per pixel CC image supported\n");
			return(-1);
		}
	}

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;
	N=ps*d;


	//First, change the surfaxis image if one was given....
	//All the intersection squares must be put inside a cube


	if(surfaxis!=NULL)
	{
		i=0;
		for(z=0; z<d; z++)
			for(y=0; y<cs; y++)
				for(x=0; x<rs; x++)
				{
					if(UCHARDATA(surfaxis)[i] != 0)
					{
						if( (UCHARDATA(surfaxis)[i] & CC_AX) !=0)
						{
							//Remove the edge if it is not a surface edge or border edge
							r=cca_cardinal_containers(surfaxis, i, x, y, z, CC_AX, rs, ps);
							if(r>2)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AX;
						}

						if( (UCHARDATA(surfaxis)[i] & CC_AY) !=0)
						{
							r=cca_cardinal_containers(surfaxis, i, x, y, z, CC_AY, rs, ps);
							if(r>2)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AY;
						}

						if( (UCHARDATA(surfaxis)[i] & CC_AZ) !=0)
						{
							r=cca_cardinal_containers(surfaxis, i, x, y, z, CC_AZ, rs, ps);
							if(r>2)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AZ;
						}
					}
					i++;
				}

		//Finally, put in a cube the faces which don't contain all their edge (means the one which were in intersection of surfaces)
		i=0;
		for(z=0; z<d; z++)
			for(y=0; y<cs; y++)
				for(x=0; x<rs; x++)
				{
					if ((UCHARDATA(surfaxis)[i]&CC_FXY)!=0)
					{
						if( (UCHARDATA(surfaxis)[i]&CC_AX)==0 || (UCHARDATA(surfaxis)[i]&CC_AY)==0 || (UCHARDATA(surfaxis)[i+rs]&CC_AX)==0 || (UCHARDATA(surfaxis)[i+1]&CC_AY)==0)
						{
							if(z<d-1 && (UCHARDATA(image)[i]&CC_VOL)!=0) UCHARDATA(surfaxis)[i]|=CC_VOL;
							if(z>0 && (UCHARDATA(image)[i-ps]&CC_VOL)!=0) UCHARDATA(surfaxis)[i-ps]|=CC_VOL;
						}
					}


					if ((UCHARDATA(surfaxis)[i]&CC_FXZ)!=0)
					{
						if( (UCHARDATA(surfaxis)[i]&CC_AX)==0 || (UCHARDATA(surfaxis)[i]&CC_AZ)==0 || (UCHARDATA(surfaxis)[i+ps]&CC_AX)==0 || (UCHARDATA(surfaxis)[i+1]&CC_AZ)==0)
						{
							if(y<cs-1 && (UCHARDATA(image)[i]&CC_VOL)!=0) UCHARDATA(surfaxis)[i]|=CC_VOL;
							if(y>0 && (UCHARDATA(image)[i-rs]&CC_VOL)!=0) UCHARDATA(surfaxis)[i-rs]|=CC_VOL;
						}
					}

					if ((UCHARDATA(surfaxis)[i]&CC_FYZ)!=0)
					{
						if( (UCHARDATA(surfaxis)[i]&CC_AZ)==0 || (UCHARDATA(surfaxis)[i]&CC_AY)==0 || (UCHARDATA(surfaxis)[i+rs]&CC_AZ)==0 || (UCHARDATA(surfaxis)[i+ps]&CC_AY)==0)
						{
							if(x<rs-1 && (UCHARDATA(image)[i]&CC_VOL)!=0) UCHARDATA(surfaxis)[i]|=CC_VOL;
							if(x>0 && (UCHARDATA(image)[i-1]&CC_VOL)!=0) UCHARDATA(surfaxis)[i-1]|=CC_VOL;
						}
					}

					i++;
				}



		cca_makecomplex(surfaxis);
	}

	if(surfaxis==NULL)
		surfaxis=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);

	if(surfaxis!=NULL && inhibit!=NULL)
		for(i=0; i<N; i++)
			UCHARDATA(surfaxis)[i]|=UCHARDATA(inhibit)[i];


	//*******************************************************
	//Perform collapse...
	//*******************************************************

	//Try to collapse and avoid creating intersections...
	if(cca_collapse_method_facesandsubfaces_avoid_intersections(image, surfaxis)<0)
	{
		fprintf(stderr, "Error: cca_collapse() failed.\n");
		freeimage(image);
		if(surfaxis!=NULL)
			freeimage(surfaxis);
		return(-1);
	}


	//Then, reload the original surfaxis, and keep the free edges of the image
	/*if(surfaxis!=NULL)
	{
		freeimage(surfaxis);
		surfaxis=readimage(argv[3]);
		i=0;
		for(z=0; z<d; z++)
			for(y=0; y<cs; y++)
				for(x=0; x<rs; x++)
				{
					if(UCHARDATA(surfaxis)[i] != 0)
					{
						if( (UCHARDATA(surfaxis)[i] & CC_AX) !=0)
						{
							//Remove the edge if it is not a surface edge or border edge
							if(cca_cardinal_containers(surfaxis, i, x, y, z, CC_AX, rs, ps)>1)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AX;
						}

						if( (UCHARDATA(surfaxis)[i] & CC_AY) !=0)
						{
							if(cca_cardinal_containers(surfaxis, i, x, y, z, CC_AY, rs, ps)>1)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AY;
						}

						if( (UCHARDATA(surfaxis)[i] & CC_AZ) !=0)
						{
							if(cca_cardinal_containers(surfaxis, i, x, y, z, CC_AZ, rs, ps)>1)
								UCHARDATA(surfaxis)[i]=UCHARDATA(surfaxis)[i]-CC_AZ;
						}
					}
					i++;
				}

		for(i=0; i<N; i++)
		{
			UCHARDATA(surfaxis)[i]&=255-CC_VOL-CC_FXY-CC_FYZ-CC_FXZ-CC_PT;
		}

		cca_makecomplex(surfaxis);

	}*/

	/*if(surfaxis!=NULL && inhibit!=NULL)
		for(i=0; i<N; i++)
			UCHARDATA(surfaxis)[i]|=UCHARDATA(inhibit)[i];*/


	/*if(surfaxis==NULL)
		surfaxis=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);


	if(inhibit==NULL)
	{
		inhibit=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);
		for(i=0; i<N; i++)
		{
			if( ((UCHARDATA(image)[i] & CC_FXY)!=0) && ((UCHARDATA(surfaxis)[i] & CC_FXY)!=0))
				if(cca_cardinal_containers(image, i, getxfrompixnum(i, rs, ps), getyfrompixnum(i, rs, ps), getzfrompixnum(i, rs, ps), CC_FXY, rs, ps)==0)
				{
					UCHARDATA(inhibit)[i] |= CC_FXY;
				}

			if( ((UCHARDATA(image)[i] & CC_FXZ)!=0) && ((UCHARDATA(surfaxis)[i] & CC_FXZ)!=0))
				if(cca_cardinal_containers(image, i, getxfrompixnum(i, rs, ps), getyfrompixnum(i, rs, ps), getzfrompixnum(i, rs, ps), CC_FXZ, rs, ps)==0)
				{
					UCHARDATA(inhibit)[i] |= CC_FXZ;
				}

			if( ((UCHARDATA(image)[i] & CC_FYZ)!=0) && ((UCHARDATA(surfaxis)[i] & CC_FYZ)!=0))
				if(cca_cardinal_containers(image, i, getxfrompixnum(i, rs, ps), getyfrompixnum(i, rs, ps), getzfrompixnum(i, rs, ps), CC_FYZ, rs, ps)==0)
				{
					UCHARDATA(inhibit)[i] |= CC_FYZ;
				}
		}

		cca_makecomplex(inhibit);
	}*/

	if(surfaxis!=NULL)
		freeimage(surfaxis);

	if(inhibit==NULL)
		inhibit=allocimage(NULL, rs, cs, d, VFF_TYP_1_BYTE);

	//writeimage(inhibit, "inhib.cc");
	//writeimage(image, "debug.cc");


	//We collapse
	if(cca_collapse_method_facesandsubfaces_avoid_intersections(image, inhibit)<0)
	{
		fprintf(stderr, "Error: cca_collapse() failed.\n");
		freeimage(image);
		if(surfaxis!=NULL)
			freeimage(surfaxis);
		return(-1);
	}

	writeimage(image, argv[2]);




	if(cca_collapse_cubes_avoid_create_surfaces(image, inhibit)<0)
	{
		fprintf(stderr, "Error: cca_collapse() failed.\n");
		freeimage(image);
		if(surfaxis!=NULL)
			freeimage(surfaxis);
		return(-1);
	}




	if(inhibit!=NULL)
		freeimage(inhibit);


/*	for(i=0; i<N; i++)
	{
		if( (UCHARDATA(image)[i] & CC_VOL) != 0 )
			cca_remove_complexface(image, CC_VOL, i, rs, ps);
	}

	cca_makecomplex(image);
*/
/*

	for(i=0; i<N; i++)
	{
		if( ((UCHARDATA(image)[i]&CC_FXZ)!=0))
		{
			cx=i%rs;
			cy=(i%ps)/rs;
			cz=i/ps;
			if(	cca_cardinal_containers(image, i, cx, cy, cz, CC_FXZ, rs, ps)==0 &&
				cca_cardinal_containers(image, i, cx, cy, cz, CC_AX, rs, ps)>2 &&
				cca_cardinal_containers(image, i, cx, cy, cz, CC_AZ, rs, ps)>2 &&
				cca_cardinal_containers(image, i+1, cx+1, cy, cz, CC_AZ, rs, ps)>2 &&
				cca_cardinal_containers(image, i+ps, cx, cy, cz+1, CC_AX, rs, ps)>2)
				{
					fprintf(stdout, "Bouh %d\n", i);
				}
		}
	}*/



	writeimage(image, argv[2]);

	//****************************************************
	//Program ends
	//****************************************************
	freeimage(image);



	return(0);
}


