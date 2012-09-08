/*Les librairies standardes*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//Patch des types
#include <patch_types.h>

/*We code edges in a cube as following


                   _______0_______
                  /              /|
				3              1  |
			   /_______2______ /  4
			   |   7           |  |
			   |               |  |
			   6        8      5  /
			   | 11            | 9
			   |____10_________|/
*/

uint32_t count_cycles_in_cube2(uint32_t configuration)
{
	uint32_t i, exploration_deb, exploration_fin, component, num_edges, num_vertices, j;
	uint32_t tab_edges[12], exploration[12];
	uint32_t voisin[12][4] = {{1,3,4,7}, {0,2,4,5},{1,3,5,6},{0,2,6,7},{0,1,8,9},{1,2,9,10},{2,3,10,11},{0,3,8,11},{4,7,9,11},{4,5,8,10},{5,6,9,11},{6,7,8,10}};


	for(i=0; i<12;i++)
	{
		tab_edges[i]=configuration%2;
		configuration=configuration/2;
	}

	num_vertices=0;
	if(tab_edges[0]!=0 || tab_edges[3]!=0 || tab_edges[7]!=0) num_vertices++;
	if(tab_edges[0]!=0 || tab_edges[1]!=0 || tab_edges[4]!=0) num_vertices++;
	if(tab_edges[1]!=0 || tab_edges[2]!=0 || tab_edges[5]!=0) num_vertices++;
	if(tab_edges[2]!=0 || tab_edges[3]!=0 || tab_edges[6]!=0) num_vertices++;
	if(tab_edges[7]!=0 || tab_edges[8]!=0 || tab_edges[11]!=0) num_vertices++;
	if(tab_edges[4]!=0 || tab_edges[8]!=0 || tab_edges[9]!=0) num_vertices++;
	if(tab_edges[6]!=0 || tab_edges[10]!=0 || tab_edges[11]!=0) num_vertices++;
	if(tab_edges[5]!=0 || tab_edges[9]!=0 || tab_edges[10]!=0) num_vertices++;

	exploration_deb=0;
	exploration_fin=0;

	component=0;
	i=0;
	num_edges=0;
	while(i<12)
	{
		if(tab_edges[i]!=0)
		{
			component++;
			exploration_deb=0;
			exploration_fin=1;
			exploration[0]=i;
			tab_edges[i]=0;

			while(exploration_deb!=exploration_fin)
			{
				for(j=0; j<4; j++)
				{
					if(tab_edges[voisin[exploration[exploration_deb]][j]]!=0)
					{
						tab_edges[voisin[exploration[exploration_deb]][j]]=0;
						exploration[exploration_fin++]=voisin[exploration[exploration_deb]][j];
					}
				}
				exploration_deb++;
			}
			num_edges+=exploration_fin;
		}
		i++;
	}

	return(num_edges+component-num_vertices);
}


uint32_t count_isolated_faces2(uint32_t code_faces, uint32_t code_surf_edges)
{
	uint32_t i, exploration_deb, exploration_fin, j, face, edge, num_isolated_surfaces;
	uint32_t tab_faces[6], tab_edges[12], exploration[6];
	uint32_t neighbouring_edges_of_face[6][4] = {{0,1,2,3},{0,4,7,8},{3,6,7,11},{8,9,10,11},{2,5,6,10},{1,4,5,9}};
	uint32_t neighbouring_faces_of_edge[12][2] = {{0,1},{0,5},{0,4},{0,2},{1,5},{4,5},{2,4},{1,2},{1,3},{3,5},{3,4},{2,3}};

	for(i=0; i<6; i++)
	{
		tab_faces[i]=code_faces%2;
		code_faces/=2;
	}


	//si tab_intersect_edge[i] vaut 1, alors le edge i (s'il appartient à une face) permet de passer d'une face à l'autre...
	//Sinon, c'est un edge s'intersection de surface, on ne peut passer par lui pour passer d'une face à l'autre...
	for(i=0; i<12;i++)
	{
		tab_edges[i]=code_surf_edges%2;
		code_surf_edges/=2;
	}

	//Sorry for french...
	//The goal of this program is to check if any face (or group of faces) of the cube is going to be "isolated" from outside world
	//by some intersection edges
	//code of intersection_edge: the edge is an intersection edge or isolated >> tab_intersect[i]=0
	//the edge is a surface edge : either its two container are in the cube or one of its container is outside the cube >>code 1
	//in this last case, we consider the face containing this edge and all its neighbours and remove them.


	i=0;
	//Look for all the faces which have a surface edge but only one neighbouring face in the cube... It means this face is neighbour with a face outside the cube
	//Develop all its neighbours and remove them.
	while(i<12)
	{
		if(	tab_edges[i]==1 &&
			((tab_faces[neighbouring_faces_of_edge[i][0]] & tab_faces[neighbouring_faces_of_edge[i][1]])==0) &&
			((tab_faces[neighbouring_faces_of_edge[i][0]] | tab_faces[neighbouring_faces_of_edge[i][1]])==1))
		{
			//Which face is the one present in the cube?
			if(tab_faces[neighbouring_faces_of_edge[i][0]]!=0)
				face=neighbouring_faces_of_edge[i][0];
			else
				face=neighbouring_faces_of_edge[i][1];

			exploration[0]=face;
			exploration_deb=0;
			exploration_fin=1;
			tab_faces[face]=0;

			//Explore all neighbours of face which share a surfacic edge, and all their own neighbours...
			while(exploration_deb!=exploration_fin)
			{
				for(j=0; j<4; j++)
				{
					edge=neighbouring_edges_of_face[exploration[exploration_deb]][j];
					if(tab_edges[edge]==1)
					{
						//The edge is a surfacic edge... develop the face neighbouring of our face with this edge
						if(tab_faces[neighbouring_faces_of_edge[edge][0]]!=0)
						{
							exploration[exploration_fin++]=neighbouring_faces_of_edge[edge][0];
							tab_faces[neighbouring_faces_of_edge[edge][0]]=0;
						}
						else if(tab_faces[neighbouring_faces_of_edge[edge][1]]!=0)
						{
							exploration[exploration_fin++]=neighbouring_faces_of_edge[edge][1];
							tab_faces[neighbouring_faces_of_edge[edge][1]]=0;
						}
					}
				}
				exploration_deb++;
			}
		}
		i++;
	}

	//Now, explore all remaining faces in order to find out how many isolated surfaces were created
	num_isolated_surfaces=0;
	i=0;
	while(i<6)
	{
		if(tab_faces[i]==1)
		{
			exploration[0]=i;
			exploration_deb=0;
			exploration_fin=1;
			tab_faces[i]=0;

			//Explore all neighbours of face which share a surfacic edge, and all their own neighbours...
			while(exploration_deb!=exploration_fin)
			{
				for(j=0; j<4; j++)
				{
					edge=neighbouring_edges_of_face[exploration[exploration_deb]][j];
					if(tab_edges[edge]==1)
					{
						//The edge is a surfacic edge... develop the face neighbouring of our face with this edge
						if(tab_faces[neighbouring_faces_of_edge[edge][0]]!=0)
						{
							exploration[exploration_fin++]=neighbouring_faces_of_edge[edge][0];
							tab_faces[neighbouring_faces_of_edge[edge][0]]=0;
						}
						else if(tab_faces[neighbouring_faces_of_edge[edge][1]]!=0)
						{
							exploration[exploration_fin++]=neighbouring_faces_of_edge[edge][1];
							tab_faces[neighbouring_faces_of_edge[edge][1]]=0;
						}
					}
				}
				exploration_deb++;
			}
			num_isolated_surfaces++;
		}
		i++;
	}

	return(num_isolated_surfaces);
}




int main()
{
	uint32_t t, max_trouv;
	max_trouv=0;
	t=0;

	fprintf(stdout, "%d\n", count_isolated_faces2(59,1705));
	return(0);
}
