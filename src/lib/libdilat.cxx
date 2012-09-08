//Librairies standardes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//Patch des types
#include <patch_types.h>


uint32_t 2d_dilat_discrete(struct xvimage* image, uint32_t mode)
{
	uint32_t rs, cs, t, cont, N;
	int32_t v;
	struct xvimage *image2, *temp;

	rs=rowsize(image);
	cs=colsize(image);
	N=rs*cs;

	if(mode!=4 && mode!=8)
	{
		fprintf(stderr, "2d_dilat_discrete(): Unsupported mode given as parameter.\n")
		return(0);
	}

	switch(datatype(image))
	{
		case (VFF_TYP_4_BYTE): //long image
			image2=allocimage(NULL, rs, cs, 1, VFF_TYP_4_BYTE);
			if(image2==NULL)
			{
				fprintf(stderr, "2d_dilat_discrete(): Memory allocation error.\n");
				return(0);
			}

			t=0;
			cont=1;
			while(cont!=0)
			{
				cont=0;
				pix=0;
				for(j=0; j<cs; j++)
					for(i=0; i<rs; i++)
					{
						if(ULONGDATA(image)[pix]>=t)
						{
							cont=1;

							if(ULONGDATA(image)[pix]>ULONGDATA(image2)[pix])
								ULONGDATA(image2)[pix]=ULONGDATA(image)[pix];

							for(i=0; i<8; i=i+2)
							{
								v=voisin(pix, i, rs, N);
								if(v>0)
								{
									if(ULONGDATA(image)[v]<ULONGDATA(image)[pix])
										ULONGDATA(image2)[v]=ULONGDATA(image)[pix];
								}
							}

							if(mode==8) //four more points to scan
							{
								for(i=1; i<8; i=i+2)
								{
									v=voisin(pix, i, rs, N);
									if(v>0)
									{
										if(ULONGDATA(image)[v]<ULONGDATA(image)[pix])
											ULONGDATA(image2)[v]=ULONGDATA(image)[pix];
									}
								}
							}
						}
						pix++;
					}

				t++;
				temp=image;
				image=image2;
				image2=temp;
			}

			break;

		case(VFF_TYP_FLOAT): //float image
			image2=allocimage(NULL, rs, cs, 1, VFF_TYP_4_BYTE);
			if(image2==NULL)
			{
				fprintf(stderr, "2d_dilat_discrete(): Memory allocation error.\n");
				return(0);
			}

			t=0;
			cont=1;
			while(cont!=0)
			{
				cont=0;
				pix=0;
				for(j=0; j<cs; j++)
					for(i=0; i<rs; i++)
					{
						if(FLOATDATA(image)[pix]>=t)
						{
							cont=1;

							if(FLOATDATA(image)[pix]>FLOATDATA(image2)[pix])
								FLOATDATA(image2)[pix]=FLOATDATA(image)[pix];

							for(i=0; i<8; i=i+2)
							{
								v=voisin(pix, i, rs, N);
								if(v>0)
								{
									if(FLOATDATA(image)[v]<FLOATDATA(image)[pix])
										FLOATDATA(image2)[v]=FLOATDATA(image)[pix];
								}
							}

							if(mode==8) //four more points to scan
							{
								for(i=1; i<8; i=i+2)
								{
									v=voisin(pix, i, rs, N);
									if(v>0)
									{
										if(FLOATDATA(image)[v]<FLOATDATA(image)[pix])
											FLOATDATA(image2)[v]=FLOATDATA(image)[pix];
									}
								}
							}
						}
						pix++;
					}

				t++;
				temp=image;
				image=image2;
				image2=temp;
			}

			break;

		default: //Unsupported
			fprintf(stderr, "2d_dilat_discrete(): Unsupported image given.\n")
			return(NULL);
			break;
	}

	freeimage(temp);
	return(image);
}



int32_t 3d_dilat_discrete(struct xvimage* image, uint32_t mode)
{
	uint32_t rs, cs, t, cont, N;
	int32_t v;
	struct xvimage *image2, *temp;

	rs=rowsize(image);
	cs=colsize(image);
	N=rs*cs;

	if(mode!=6 && mode!=26)
	{
		fprintf(stderr, "3d_dilat_discrete(): Unsupported mode given as parameter.\n")
		return(0);
	}

	switch(datatype(image))
	{
		case (VFF_TYP_4_BYTE): //long image
			image2=allocimage(NULL, rs, cs, 1, VFF_TYP_4_BYTE);
			if(image2==NULL)
			{
				fprintf(stderr, "2d_dilat_discrete(): Memory allocation error.\n");
				return(0);
			}

			t=0;
			cont=1;
			while(cont!=0)
			{
				cont=0;
				pix=0;
				for(j=0; j<cs; j++)
					for(i=0; i<rs; i++)
					{
						if(ULONGDATA(image)[pix]>=t)
						{
							cont=1;

							if(ULONGDATA(image)[pix]>ULONGDATA(image2)[pix])
								ULONGDATA(image2)[pix]=ULONGDATA(image)[pix];

							for(i=0; i<8; i=i+2)
							{
								v=voisin(pix, i, rs, N);
								if(v>0)
								{
									if(ULONGDATA(image)[v]<ULONGDATA(image)[pix])
										ULONGDATA(image2)[v]=ULONGDATA(image)[pix];
								}
							}

							if(mode==8) //four more points to scan
							{
								for(i=1; i<8; i=i+2)
								{
									v=voisin(pix, i, rs, N);
									if(v>0)
									{
										if(ULONGDATA(image)[v]<ULONGDATA(image)[pix])
											ULONGDATA(image2)[v]=ULONGDATA(image)[pix];
									}
								}
							}
						}
						pix++;
					}

				t++;
				temp=image;
				image=image2;
				image2=temp;
			}

			break;

		case(VFF_TYP_FLOAT): //float image
			image2=allocimage(NULL, rs, cs, 1, VFF_TYP_4_BYTE);
			if(image2==NULL)
			{
				fprintf(stderr, "2d_dilat_discrete(): Memory allocation error.\n");
				return(0);
			}

			t=0;
			cont=1;
			while(cont!=0)
			{
				cont=0;
				pix=0;
				for(j=0; j<cs; j++)
					for(i=0; i<rs; i++)
					{
						if(FLOATDATA(image)[pix]>=t)
						{
							cont=1;

							if(FLOATDATA(image)[pix]>FLOATDATA(image2)[pix])
								FLOATDATA(image2)[pix]=FLOATDATA(image)[pix];

							for(i=0; i<8; i=i+2)
							{
								v=voisin(pix, i, rs, N);
								if(v>0)
								{
									if(FLOATDATA(image)[v]<FLOATDATA(image)[pix])
										FLOATDATA(image2)[v]=FLOATDATA(image)[pix];
								}
							}

							if(mode==8) //four more points to scan
							{
								for(i=1; i<8; i=i+2)
								{
									v=voisin(pix, i, rs, N);
									if(v>0)
									{
										if(FLOATDATA(image)[v]<FLOATDATA(image)[pix])
											FLOATDATA(image2)[v]=FLOATDATA(image)[pix];
									}
								}
							}
						}
						pix++;
					}

				t++;
				temp=image;
				image=image2;
				image2=temp;
			}

			break;

		default: //Unsupported
			fprintf(stderr, "2d_dilat_discrete(): Unsupported image given.\n")
			return(NULL);
			break;
	}

	freeimage(temp);
	return(image);
}


int32_t 2d_dilat_euclidean()


int32_t 3d_dilat_euclidean()
