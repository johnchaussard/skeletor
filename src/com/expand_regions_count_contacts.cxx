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





int32_t main(int argc, char* argv[])
{
	struct xvimage *regions;
	uint32_t v, rs, cs, d, ps, N, i, k, j, nbcomp, **tab_results, num_cont, num_color, color[6], *tab_contacts;


	regions=readimage(argv[1]);

	rs=rowsize(regions);
	cs=colsize(regions);
	d=depth(regions);
	ps=rs*cs;
	N=d*ps;


	nbcomp=0;num_cont=0;
	for(i=0; i<N; i++)
		if(ULONGDATA(regions)[i]>nbcomp)
			nbcomp=ULONGDATA(regions)[i];


	tab_results=(uint32_t**)calloc(nbcomp+1, sizeof(uint32_t*));
	for(i=0; i<nbcomp+1; i++)
	{
		tab_results[i]=(uint32_t*)calloc(nbcomp+1, sizeof(uint32_t));
	}


	for(i=0; i<N; i++)
		if(ULONGDATA(regions)[i]==0)
		{
			num_color=0;
			for(k=0; k<=10; k=k+2)
			{
				v=voisin6(i, k, rs, ps, N);
				if(v>=0 && ULONGDATA(regions)[v]!=0)
				{
					color[num_color]=ULONGDATA(regions)[v];
					num_color++;
				}
			}

			for(k=0; k<num_color; k++)
				for(v=k+1; v<num_color; v++)
				{
					if(color[k]==color[v])
					{
						color[v]=color[num_color-1];
						num_color--;
						v--;
					}
				}

			for(k=0; k<num_color; k++)
				for(v=k+1; v<num_color; v++)
					if(color[k] > color[v])
					{
						j=color[k];
						color[k]=color[v];
						color[v]=j;
					}

			for(k=0; k<num_color; k++)
				for(v=k+1; v<num_color; v++)
				{
					if(tab_results[color[k]][color[v]] ==0)
						num_cont++;

					tab_results[color[k]][color[v]]++;
				}
		}


	tab_contacts=(uint32_t*)calloc(num_cont, sizeof(uint32_t));

	j=0;
	for(k=0; k<nbcomp+1; k++)
		for(v=0; v<nbcomp+1; v++)
		{
			if(tab_results[k][v]!=0)
				tab_contacts[j++]=tab_results[k][v];
		}

	for(k=0; k<num_cont; k++)
		for(v=k+1; v<num_cont; v++)
		{
			if(tab_contacts[k]>tab_contacts[v])
			{
				j=tab_contacts[k];
				tab_contacts[k]=tab_contacts[v];
				tab_contacts[v]=j;
			}
		}



	fprintf(stdout, "%d\n", num_cont);
	for(k=0; k<num_cont; k++)
		fprintf(stdout, "%d\n", tab_contacts[k]);




	return(0);
}
