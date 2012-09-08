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




typedef struct point{
	uint32_t x;
	uint32_t y;
	uint32_t z;
}point;




//Fonction naive qui dessine un cube de cote rayon_carre, centre sur centre, dans image
void draw_a_cube(struct xvimage *image, point centre, uint32_t rayon_carre)
{
	int32_t rs, cs, d, ps, n, i, j, z, jmin, jmax, imin, imax, zmin, zmax;
	rs=rowsize(image);
	cs=colsize(image);
	d=depth(image);
	ps=rs*cs;


	jmin=centre.y - rayon_carre+1;
	if (jmin<0) jmin=0;
	jmax=centre.y + rayon_carre-1;
	if (jmax>cs-1) jmax=cs-1;


	imin=centre.x - rayon_carre+1;
	if (imin<0) imin=0;
	imax=centre.x + rayon_carre-1;
	if (imax>rs-1) imax=rs-1;

	zmin=centre.z - rayon_carre+1;
	if (zmin<0) zmin=0;
	zmax=centre.z + rayon_carre-1;
	if (zmax>d-1) zmax=d-1;

	for(z=zmin; z<=zmax; z++)
		for(j=jmin; j<=jmax; j++)
			for(i=imin; i<=imax; i++)
			{
				n=z*ps+j*rs+i;

				if(ULONGDATA(image)[n]<rayon_carre)
				{
					ULONGDATA(image)[n]=rayon_carre;

				}
			}
}



//On effectue la fonction d'ouverture sur une ligne, vers la droite
void dilate_one_line_right(struct xvimage* image, uint32_t num_ligne, uint32_t num_plan, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	ps=colsize(image)*rs;

	max=0; maxcentre=0;

	//On parcourt toute la ligne
	for(i=0; i<rs; i++)
	{
			//Le carré que l'on dilate actuellement ne doit plus être dilaté
			if(maxcentre+max == i)
			{
				//Si on a dans notre liste un atre carré à dilater, on le prend
				if(pointeur_fin!=pointeur_debut)
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				//Sinon, on dilate le carré où on est
				else
				{
					max=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? On vide le tableau de liste des carres plus petits que lui
			//Ici, on peut croire que l'algo n'est plus linéaire...
			//Mais en tout, on ajoute au plus n element dans ce tableau, avec n=taille de ligne
			//Ici, on ne peut donc qu'en retirer, en tout, que n au plus... donc, on reste linéaire
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][0] + tab_tri[pointeur_fin-1][1]) < (ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]+i)) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
				tab_tri[pointeur_fin++][1]=i;
			}

			//Il ne reste pas de carré plus grand... on vérifie que notre carré couvrira certains pixels
			if((pointeur_debut==pointeur_fin) && (maxcentre+max < ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]+i))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
				tab_tri[pointeur_fin++][1]=i;
			}

		//Si le pixel où l'on est contient un carré plus grand que celui que l'on propage actuellement
		if(ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]>=max)
		{
			//On remplace le carré propagé par celui contenu dans le pixel où on est
			max=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit dans la liste tab_tri
		}

		ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]=max;
	}
}



//On effectue la fonction d'ouverture sur une colonne, vers le bas
void dilate_one_line_bottom(struct xvimage* image, uint32_t num_col, uint32_t num_plan, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps, cs;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;

	max=0; maxcentre=0;

	for(i=0; i<cs; i++)
	{
			if(maxcentre+max == i) //The current square should no more be dilated
			{
				if(pointeur_fin!=pointeur_debut) //We have another square to dilate
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				else //We have no square to dilate, take current one
				{
					max=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? ON vide le tableau de liste des carres plus petits que lui
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[num_plan*ps+i*rs+num_col]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][0] + tab_tri[pointeur_fin-1][1]) < (ULONGDATA(image)[num_plan*ps+i*rs+num_col]+i)) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

			if((pointeur_debut==pointeur_fin) && (maxcentre+max < ULONGDATA(image)[num_plan*ps+i*rs+num_col]+i))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

		if(ULONGDATA(image)[num_plan*ps+i*rs+num_col]>=max) //New square bigger to dilate
		{
			max=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit
		}

		ULONGDATA(image)[num_plan*ps+i*rs+num_col]=max;
	}
}



//On effectue la fonction d'ouverture sur une profondeur, vers l'arriere
void dilate_one_line_backward(struct xvimage* image, uint32_t num_col, uint32_t num_ligne, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps, d;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	d=depth(image);
	ps=rs*colsize(image);

	max=0; maxcentre=0;

	for(i=0; i<d; i++)
	{
			if(maxcentre+max == i) //The current square should no more be dilated
			{
				if(pointeur_fin!=pointeur_debut) //We have another square to dilate
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				else //We have no square to dilate, take current one
				{
					max=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? ON vide le tableau de liste des carres plus petits que lui
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[i*ps+num_ligne*rs+num_col]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][0] + tab_tri[pointeur_fin-1][1]) < (ULONGDATA(image)[i*ps+num_ligne*rs+num_col]+i)) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

			if((pointeur_debut==pointeur_fin) && (maxcentre+max < ULONGDATA(image)[i*ps+num_ligne*rs+num_col]+i))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

		if(ULONGDATA(image)[i*ps+num_ligne*rs+num_col]>=max) //New square bigger to dilate
		{
			max=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit
		}


		ULONGDATA(image)[i*ps+num_ligne*rs+num_col]=max;
	}
}



//On effectue la fonction d'ouverture sur une ligne, vers la gauche
void dilate_one_line_left(struct xvimage* image, uint32_t num_ligne, uint32_t num_plan, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	ps=colsize(image)*rs;

	max=0; maxcentre=0;

	for(i=rs-1; i>=0; i--)
	{
			if(maxcentre-max == i) //The current square should no more be dilated
			{
				if(pointeur_fin!=pointeur_debut) //We have another square to dilate
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				else //We have no square to dilate, take current one
				{
					max=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? ON vide le tableau de liste des carres plus petits que lui
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][1] - tab_tri[pointeur_fin-1][0]) > i-(ULONGDATA(image)[num_plan*ps+num_ligne*rs+i])) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
				tab_tri[pointeur_fin++][1]=i;
			}

			if((pointeur_debut==pointeur_fin) && (maxcentre-max > i-ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
				tab_tri[pointeur_fin++][1]=i;
			}

		if(ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]>=max) //New square bigger to dilate
		{
			max=ULONGDATA(image)[num_plan*ps+num_ligne*rs+i];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit
		}

		ULONGDATA(image)[num_plan*ps+num_ligne*rs+i]=max;
	}
}



//On effectue la fonction d'ouverture sur une colonne, vers le haut
void dilate_one_line_top(struct xvimage* image, uint32_t num_col, uint32_t num_plan, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps, cs;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	cs=colsize(image);
	ps=rs*cs;

	max=0; maxcentre=0;

	for(i=cs-1; i>=0; i--)
	{
			if(maxcentre-max == i) //The current square should no more be dilated
			{
				if(pointeur_fin!=pointeur_debut) //We have another square to dilate
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				else //We have no square to dilate, take current one
				{
					max=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? ON vide le tableau de liste des carres plus petits que lui
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[num_plan*ps+i*rs+num_col]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][1] - tab_tri[pointeur_fin-1][0]) > i-(ULONGDATA(image)[num_plan*ps+i*rs+num_col])) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

			if((pointeur_debut==pointeur_fin) && (maxcentre-max > i-ULONGDATA(image)[num_plan*ps+i*rs+num_col]))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

		if(ULONGDATA(image)[num_plan*ps+i*rs+num_col]>=max) //New square bigger to dilate
		{
			max=ULONGDATA(image)[num_plan*ps+i*rs+num_col];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit
		}

		ULONGDATA(image)[num_plan*ps+i*rs+num_col]=max;
	}
}



//On effectue la fonction d'ouverture sur une profondeur, vers l'avant
void dilate_one_line_forward(struct xvimage* image, uint32_t num_col, uint32_t num_ligne, uint32_t **tab_tri)
{
	uint32_t max, i, maxcentre, rs, ps, d;
	int32_t pointeur_fin, pointeur_debut;

	pointeur_fin=0;
	pointeur_debut=0;

	rs=rowsize(image);
	d=depth(image);
	ps=rs*colsize(image);

	max=0; maxcentre=0;

	for(i=d-1; i>=0; i--)
	{
			if(maxcentre-max == i) //The current square should no more be dilated
			{
				if(pointeur_fin!=pointeur_debut) //We have another square to dilate
				{
					max=tab_tri[pointeur_debut][0];
					maxcentre=tab_tri[pointeur_debut][1];
					pointeur_debut++;
				}
				else //We have no square to dilate, take current one
				{
					max=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
					maxcentre=i;
				}
			}

			//Est-ce que le carré où l'on se trouve mérite d'être dilaté plus tard ? ON vide le tableau de liste des carres plus petits que lui
			while(pointeur_debut!=pointeur_fin && ULONGDATA(image)[i*ps+num_ligne*rs+num_col]>=tab_tri[pointeur_fin-1][0])
			{
				pointeur_fin--;
			}

			//S'il reste un carré plus grand, on vérifie que notre carré aura quand même une extension plus importante
			if((pointeur_debut!=pointeur_fin) && ((tab_tri[pointeur_fin-1][1] - tab_tri[pointeur_fin-1][0]) > i-(ULONGDATA(image)[i*ps+num_ligne*rs+num_col])) )
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

			if((pointeur_debut==pointeur_fin) && (maxcentre-max > i-ULONGDATA(image)[i*ps+num_ligne*rs+num_col]))
			{
				tab_tri[pointeur_fin][0]=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
				tab_tri[pointeur_fin++][1]=i;
			}

		if(ULONGDATA(image)[i*ps+num_ligne*rs+num_col]>=max) //New square bigger to dilate
		{
			max=ULONGDATA(image)[i*ps+num_ligne*rs+num_col];
			maxcentre=i;
			pointeur_fin=pointeur_debut; //On oublie tous les max que l'on avait construit
		}

		ULONGDATA(image)[i*ps+num_ligne*rs+num_col]=max;
	}
}





int main(int argc, char *argv[])
{
	struct xvimage *distmap, *output;
	uint32_t rs, cs, d, i, j, k, n, **tab, N, l;
	point c;
	distmap=readimage(argv[1]);



	rs=rowsize(distmap);
	cs=colsize(distmap);
	d=depth(distmap);
	N=d*cs*rs;


	//Algo naif de dilatation
	l=0;
	output=allocimage(NULL,rs, cs, d, VFF_TYP_4_BYTE);
	n=0;
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				if(ULONGDATA(distmap)[n]>0)
				{
					c.x=i; c.y=j; c.z=k;
					draw_a_cube(output, c, ULONGDATA(distmap)[n]);
				}
				if( (n*100)/N > l)
				{
					l++;
					fprintf(stdout, "%d ", l);
					fflush(stdout);
				}
				n++;
			}
	writeimage(output, (char*)"naif.pgm");

	fprintf(stdout, "Naive done\n");


	//Debut de l'algo


	//Un tableau qui sera utlisé comme zone memoire temporaire par les algos...
	//On le réalloue dès que l'on change de méthode de parcours (ligne, colonne, profondeur)
	//On l'alloue pour le parcours des lignes
	tab=(uint32_t**)calloc(rs, sizeof(uint32_t*));
	for(i=0; i<rs; i++)
		tab[i]=(uint32_t*)calloc(2, sizeof(uint32_t));


	//On parcourt les lignes... de gauche à droite, puis droite à gauche
	for(k=0; k<d; k++)
		for(j=0; j<cs; j++)
		{
			dilate_one_line_right(distmap, j, k, tab);
			dilate_one_line_left(distmap, j, k, tab);
		}

	//On détruit le tableau
	for(i=0; i<rs; i++)
		free(tab[i]);
	free(tab);




	//On réalloue le tableau pour les colonnes
	tab=(uint32_t**)calloc(cs, sizeof(uint32_t*));
	for(i=0; i<cs; i++)
		tab[i]=(uint32_t*)calloc(2, sizeof(uint32_t));


	//On parcourt les colonnes, haut n bas, puis bas en haut
	for(k=0; k<d; k++)
		for(i=0; i<rs; i++)
		{
			dilate_one_line_bottom(distmap, i, k, tab);
			dilate_one_line_top(distmap, i, k, tab);
		}


	for(i=0; i<cs; i++)
		free(tab[i]);
	free(tab);


	//If 3d
	if(d>1)
	{
		//Alloue le tableau
		tab=(uint32_t**)calloc(d, sizeof(uint32_t*));
		for(i=0; i<d; i++)
			tab[i]=(uint32_t*)calloc(2, sizeof(uint32_t));

		//Parcours des profondeurs, de avant vers arriere, puis inversement
		for(j=0; j<cs; j++)
			for(i=0; i<rs; i++)
			{
				dilate_one_line_backward(distmap, i, j, tab);
				dilate_one_line_forward(distmap, i, j, tab);
			}

		for(i=0; i<d; i++)
			free(tab[i]);
		free(tab);
	}

	//On écrit le résultat
	writeimage(distmap, (char*)"result.pgm");


	//Comparaison des résultats algo naif avec algo perso
	n=rs*cs*d;
	for(i=0; i<n; i++)
		if(ULONGDATA(distmap)[i]!=ULONGDATA(output)[i])
		{
			fprintf(stdout, "%d %d %d %d\n", i%rs, i/rs, ULONGDATA(distmap)[i], ULONGDATA(output)[i]);
			return(-1);
		}


	fprintf(stdout, "Done\n");

	freeimage(distmap);
	freeimage(output);



	return(0);
}






