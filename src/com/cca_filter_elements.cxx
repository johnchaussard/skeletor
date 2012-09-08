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

//coeff should be inclusive for keeping
#define USAGE "<cca_input_image> <cca_output_image> <collapse|size|holding> <coeff> (<additional_image>)"

#define STRATEGY_COLLAPSE 1
#define STRATEGY_SIZE 2
#define STRATEGY_HOLDING 3
#define STRATEGY_HOLDING_FIX 4
#define STRATEGY_HOLDING_PERCENT 5


int32_t main(int argc, char *argv[])
{
	struct xvimage *image, *axis, *output;
	uint32_t i, N, icoeff, di, j, max;
	int32_t rs, cs, d;
	unsigned char strategy;
	complexe *t;
	list *decomposition, **l, *border_faces;

	output=NULL;


	//*******************************************
	//Checking input values
	//*******************************************
	if (argc!=5 && argc!=6)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}

	//We read input image
	image=readimage(argv[1]);
	if (image==NULL)
	{
		fprintf(stderr, "Error: Could not read %s.\n", argv[1]);
		return(1);
	}
	else if(datatype(image)!=VFF_TYP_1_BYTE)
	{
		fprintf(stderr, "Error: only CC image supported\n");
		return(1);
	}

	//Which filtering option was chosen ?
	if(strcmp(argv[3], "collapse") == 0)
	{
		strategy = STRATEGY_COLLAPSE;
	}
	else if(strcmp(argv[3], "size") == 0)
	{
		strategy = STRATEGY_SIZE;
	}
	else if(strcmp(argv[3], "holding") == 0)
	{
		strategy = STRATEGY_HOLDING;
	}
	else
	{
		freeimage(image);
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(1);
	}


	//Get the coefficient
	if(strategy==STRATEGY_COLLAPSE || strategy==STRATEGY_SIZE)
	{
		icoeff=atoi(argv[4]);
		if(icoeff <= 0)
		{
			fprintf(stderr, "Coefficient should be positive integer.\n");
			freeimage(image);
			return(1);
		}
	}
	else if(strategy==STRATEGY_HOLDING)
	{
		if(argc!=6)
		{
			freeimage(image);
			fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
			return(1);
		}

		if(argv[4][strlen(argv[4])-1]=='%')
		{
			strategy=STRATEGY_HOLDING_PERCENT;
			argv[4][strlen(argv[4])-1]='\0';
			icoeff=atoi(argv[4]);
			if(icoeff<0)
			{
				fprintf(stderr, "Coefficient should be positive value.\n");
				freeimage(image);
				return(1);
			}
		}
		else
		{
			strategy=STRATEGY_HOLDING_FIX;
			icoeff=atoi(argv[4]);
			if(icoeff<0)
			{
				fprintf(stderr, "Coefficient should be positive integer.\n");
				freeimage(image);
				return(1);
			}
		}
	}

	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	N=rs*cs*d;


	//*******************************************************
	//Decomposition of the skeleton into elements
	//*******************************************************

	//We can perform decomposition
	decomposition=cca_decompose_skeleton_into_elements(image);
	if(decomposition==NULL)
	{
		fprintf(stderr, "Error in the decomposition function.\n");
		freeimage(image);
		return(1);
	}

	decomposition->mode=MODE_FIFO;

	//Get rid of intersection elements
	t=(complexe*)list_pop_pointer(decomposition);
	complexe_free_complexe(t);

	//Careful here, image is no more the same as original data


	//******************************************************
	//Perform filtering based on the user's strategy
	//******************************************************

	switch(strategy)
	{
		case(STRATEGY_SIZE):

			output=image;

			for(i=0; i<N; i++)
			{
				UCHARDATA(output)[i]=0;
			}

			while(!list_isempty(decomposition))
			{
				t=(complexe*)list_pop_pointer(decomposition);
				di = complexe_get_dimension(t);
				if( ( (di==2) && ((t->num_fxy + t->num_fyz + t->num_fxz) >= icoeff) ) ||
					( (di==1) && ((t->num_ax + t->num_ay + t->num_az) >= icoeff) ) )
				{
					complexe_add_to_cca(output, t);
				}
				complexe_free_complexe(t);
			}
			break;


		case(STRATEGY_COLLAPSE):
			//Regenerate image
			image=readimage(argv[1]);

			border_faces=NULL;
			l=cca_collapse_directional_surfacic(image, NULL, icoeff, NULL, &output, &border_faces);
			freeimage(output);
			list_delete(border_faces, NO_FREE_DATA);

			if(l==NULL)
			{
				fprintf(stderr, "Error during collapse.\n");
				freeimage(image);
				return(1);
			}

			output=image;

			for(i=0; i<24; i++)
			{
				list_delete(l[i],NO_FREE_DATA);
			}
			free(l);

			while(!list_isempty(decomposition))
			{
				t=(complexe*)list_pop_pointer(decomposition);
				complexe_keep_highest_dimension_faces(t);

				if(complexe_is_partly_inside_cca(t, image))
				{
					complexe_add_to_cca(output, t);
				}

				complexe_free_complexe(t);
			}
			break;


		case(STRATEGY_HOLDING_PERCENT):
		case(STRATEGY_HOLDING_FIX):
			freeimage(image);

			axis=readimage(argv[5]);
			if(axis==NULL)
			{
				fprintf(stderr, "Error: Could not read %s.\n", argv[5]);
				return(1);
			}
			else if(datatype(axis)!=VFF_TYP_1_BYTE)
			{
				fprintf(stderr, "Error: only CC image supported for axis image.\n");
				return(1);
			}
			else if(rowsize(axis)!=rs || colsize(axis)!=cs || depth(axis)!=d)
			{
				fprintf(stderr, "Error: original image and axis image must be same size.\n");
				return(1);
			}

			output=axis;

			max=list_getsize(decomposition);
			for(i=0; i<max; i++)
			{
				t=(complexe*)list_pop_pointer(decomposition);
				complexe_keep_highest_dimension_faces(t);

				di=complexe_count_faces_inside_cca(t, axis);

				if(strategy == STRATEGY_HOLDING_FIX)
				{
					if(di>= icoeff) list_push_pointer(decomposition, t);
					else complexe_free_complexe(t);
				}
				else
				{
					j=complexe_get_num_faces(t);
					if(di*100 >= icoeff*j) list_push_pointer(decomposition, t);
					else complexe_free_complexe(t);
				}
			}

			for(i=0; i<N; i++)
			{
				UCHARDATA(output)[i]=0;
			}

			while(!list_isempty(decomposition))
			{
				t=(complexe*)list_pop_pointer(decomposition);
				complexe_add_to_cca(output, t);
				complexe_free_complexe(t);
			}


			break;
	}

	cca_makecomplex(output);
	writeimage(output, argv[2]);
	freeimage(output);
	list_delete(decomposition, NO_FREE_DATA);

	return(0);
}





