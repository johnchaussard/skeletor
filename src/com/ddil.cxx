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


#define PARENT(i) (i/2)
#define LEFT(i) (2*i)
#define RIGHT(i) (2*i+1)

#define TRUE 1
#define FALSE 0



typedef struct heap_element{
	void *element;
	uint32_t value;
}heap_element;

typedef struct heap{
	uint32_t size_max; //Maximal size possible
	uint32_t size; //Actual number of elements
	heap_element *tab_heap; //The array used as a heap
}heap;



typedef struct point{
	uint32_t x;
	uint32_t y;
}point;

typedef struct circle{
	point centre;
	uint32_t radius_square;
}circle;







heap *heap_new(uint32_t size_max)
{
	heap *h = (heap*)calloc(1,sizeof(heap));
	h->size_max=size_max;
	h->tab_heap = (heap_element*) calloc(size_max, sizeof(heap_element));
	h->size=0;
	return(h);
}


void heap_free(heap *h)
{
	free(h->tab_heap);
	free(h);
}


heap *heap_copy(heap *original, uint32_t size)
{
	assert(size >= original->size);
	heap *r=heap_new(size);
	memcpy(&(r->tab_heap[0]), &(original->tab_heap[0]), (original->size)*sizeof(heap_element));
	r->size=original->size;
	return r;
}


void heap_add(heap *h, void* element, uint32_t value)
{
	uint32_t position;
	heap_element t;

	h->size= h->size +1;
	assert(h->size <= h->size_max);
	position=h->size;
	h->tab_heap[position-1].value=value;
	h->tab_heap[position-1].element=element;

	while(position>1 && h->tab_heap[PARENT(position)-1].value < h->tab_heap[position-1].value)
	{
		t=h->tab_heap[PARENT(position)-1];
		h->tab_heap[PARENT(position)-1]=h->tab_heap[position-1];
		h->tab_heap[position-1]=t;
		position=PARENT(position);
	}
}


void heap_reorganize(heap *h, uint32_t position)
{
	uint32_t r,l,max;
	heap_element t;

	r=RIGHT(position);
	l=LEFT(position);

	max=position;

	if(l<=h->size && h->tab_heap[l -1].value > h->tab_heap[position -1].value)
		max=l;

	if(r<=h->size && h->tab_heap[r -1].value > h->tab_heap[max -1].value)
		max=r;

	if(max!=position)
	{
		t=h->tab_heap[position -1];
		h->tab_heap[position -1] = h->tab_heap[max -1];
		h->tab_heap[max -1]=t;
		heap_reorganize(h, max);
	}
}


heap_element heap_remove_max(heap *h)
{
	heap_element res;

	assert(h->size>=1);

	res=h->tab_heap[0];
	h->tab_heap[0]=h->tab_heap[(h->size)-1];
	h->size = h->size -1;

	heap_reorganize(h, 1);

	return res;
}


uint32_t heap_getsize(heap *h)
{
	return h->size;
}


uint32_t heap_is_empty(heap *h)
{
	if (heap_getsize(h)==0)
		return TRUE;
	else
		return FALSE;
}


void heap_printheap(heap *h)
{
	uint32_t i;
	for(i=0; i<heap_getsize(h); i++)
	{
		fprintf(stdout, "%d ", h->tab_heap[i].value);
	}
	fprintf(stdout, "\n");
}


heap *heap_fusion(heap *h1, heap *h2, uint32_t size_max)
{
	heap_element max;
	heap *res;
	assert(heap_getsize(h1)+heap_getsize(h2) <= size_max);

	res=heap_new(h1->size_max+h2->size_max);

	while(heap_is_empty(h1)==FALSE)
	{
		max=heap_remove_max(h1);
		heap_add(res, max.element, max.value);
	}

	while(heap_is_empty(h2)==FALSE)
	{
		max=heap_remove_max(h2);
		heap_add(res, max.element, max.value);
	}

	return res;
}







uint32_t is_touched_by_ball(point a_tester, point centre, uint32_t rayon_carre)
{
	if( (a_tester.x - centre.x)*(a_tester.x - centre.x) + (a_tester.y - centre.y)*(a_tester.y - centre.y) <= rayon_carre)
		return TRUE;
	return FALSE;
}



void draw_a_baball(struct xvimage *image, struct xvimage *mask, point centre, uint32_t rayon_carre)
{
	point p; //Ceci n'est pas une pub pour un garage
	uint32_t rs, cs, n, i, j;
	rs=rowsize(image);
	cs=colsize(image);
	n=0;
	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			if(ULONGDATA(mask)[n]>0)
			{
				p.x=i; p.y=j;
				if(is_touched_by_ball(p, centre, rayon_carre)==TRUE && ULONGDATA(image)[n]<rayon_carre)
				{
					ULONGDATA(image)[n]=rayon_carre;
				}
			}
			n++;
		}
}






void dilate_one_line_right(circle *cercles, uint32_t num_ligne, uint32_t rs, heap **tab_heaps)
{
	circle max;
	heap_element e;
	int32_t i,j;
	heap *h, *copy, *copy2;
	point p;

	h=heap_new(rs);
	max.radius_square=0;

	for(i=0; i<rs; i++)
	{
		if(cercles[num_ligne*rs+i].radius_square>0)
		{
			if (max.radius_square!=0)
			{
				p.x=i;
				p.y=num_ligne;

				while(heap_is_empty(h)==FALSE && is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
				{
					e=heap_remove_max(h);
					max.radius_square=e.value;
					max.centre.x=((uint32_t)e.element)%rs;
					max.centre.y=((uint32_t)e.element)/rs;
				}

				if(is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
					max.radius_square=0;
			}

			if(max.radius_square==0)
			{
				max=cercles[num_ligne*rs+i];
			}
			else if(max.radius_square!=0 && cercles[num_ligne*rs+i].radius_square>=max.radius_square)
			{
				heap_add(h, (void*)(max.centre.y*rs+max.centre.x), max.radius_square);
				max=cercles[num_ligne*rs+i];
			}
			else
			{
				heap_add(h, (void*)(cercles[num_ligne*rs+i].centre.y *rs +cercles[num_ligne*rs+i].centre.x), cercles[num_ligne*rs+i].radius_square);
			}

			if(heap_is_empty(h)==FALSE)
			{
				copy=heap_copy(h, heap_getsize(h));

				if(tab_heaps[num_ligne*rs+i]==NULL)
				{
					tab_heaps[num_ligne*rs+i]=copy;
				}
				else
				{
					copy2=heap_fusion(copy, tab_heaps[num_ligne*rs+i], heap_getsize(copy)+heap_getsize(tab_heaps[num_ligne*rs+i]));
					heap_free(copy);
					heap_free(tab_heaps[num_ligne*rs+i]);
					tab_heaps[num_ligne*rs+i]=copy2;
				}
			}

			cercles[num_ligne*rs+i]=max;
		}
	}

	heap_free(h);
}


void dilate_one_line_left(circle *cercles, uint32_t num_ligne, uint32_t rs, heap **tab_heaps)
{
	circle max;
	heap_element e;
	int32_t i,j;
	heap *h, *copy, *copy2;
	point p;

	h=heap_new(rs);
	max.radius_square=0;

	for(i=rs-1; i>=0; i--)
	{
		if(cercles[num_ligne*rs+i].radius_square>0)
		{
			if (max.radius_square!=0)
			{
				p.x=i;
				p.y=num_ligne;

				while(heap_is_empty(h)==FALSE && is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
				{
					e=heap_remove_max(h);
					max.radius_square=e.value;
					max.centre.x=((uint32_t)e.element)%rs;
					max.centre.y=((uint32_t)e.element)/rs;
				}

				if(is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
					max.radius_square=0;
			}

			if(max.radius_square==0)
			{
				max=cercles[num_ligne*rs+i];
			}
			else if(max.radius_square!=0 && cercles[num_ligne*rs+i].radius_square>=max.radius_square)
			{
				heap_add(h, (void*)(max.centre.y*rs+max.centre.x), max.radius_square);
				max=cercles[num_ligne*rs+i];
			}
			else
			{
				heap_add(h, (void*)(cercles[num_ligne*rs+i].centre.y *rs +cercles[num_ligne*rs+i].centre.x), cercles[num_ligne*rs+i].radius_square);
			}

			if(heap_is_empty(h)==FALSE)
			{
				copy=heap_copy(h, heap_getsize(h));

				if(tab_heaps[num_ligne*rs+i]==NULL)
				{
					tab_heaps[num_ligne*rs+i]=copy;
				}
				else
				{
					copy2=heap_fusion(copy, tab_heaps[num_ligne*rs+i], heap_getsize(copy)+heap_getsize(tab_heaps[num_ligne*rs+i]));
					heap_free(copy);
					heap_free(tab_heaps[num_ligne*rs+i]);
					tab_heaps[num_ligne*rs+i]=copy2;
				}
			}

			cercles[num_ligne*rs+i]=max;
		}
	}

	heap_free(h);
}


void dilate_one_column_bottom(circle *cercles, uint32_t num_col, uint32_t rs, uint32_t cs, heap **tab_heaps)
{
	circle max, newmax;
	heap_element e;
	int32_t i,j;
	heap *h, *temp, *copy, *copy2;
	point p;

	h=heap_new(cs);
	max.radius_square=0;

	for(i=0; i<cs; i++)
	{
		if(tab_heaps[i*rs+num_col]!=NULL)
		{
			temp=heap_fusion(h, tab_heaps[i*rs+num_col], h->size_max+heap_getsize(tab_heaps[i*rs+num_col]));
			heap_free(h);
			heap_free(tab_heaps[i*rs+num_col]);
			tab_heaps[i*rs+num_col]=NULL;
			h=temp;

			e=heap_remove_max(h);
			if(e.value > max.radius_square)
			{
				newmax.radius_square=e.value;
				newmax.centre.x=((uint32_t)e.element)%rs;
				newmax.centre.y=((uint32_t)e.element)/rs;;
				heap_add(h, (void*)(max.centre.y*rs + max.centre.x), max.radius_square);
				max=newmax;
			}
			else
			{
				heap_add(h, e.element, e.value);
			}
		}




		if(cercles[i*rs+num_col].radius_square>0)
		{
			if (max.radius_square!=0)
			{
				p.x=num_col;
				p.y=i;

				while(heap_is_empty(h)==FALSE && is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
				{
					e=heap_remove_max(h);
					max.radius_square=e.value;
					max.centre.x=((uint32_t)e.element)%rs;
					max.centre.y=((uint32_t)e.element)/rs;
				}

				if(is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
					max.radius_square=0;
			}

			if(max.radius_square==0)
			{
				max=cercles[i*rs+num_col];
			}
			else if(max.radius_square!=0 && cercles[i*rs+num_col].radius_square>=max.radius_square)
			{
				heap_add(h, (void*)(max.centre.y *rs +max.centre.x), max.radius_square);
				max=cercles[i*rs+num_col];
			}
			else
			{
				heap_add(h, (void*)(cercles[i*rs+num_col].centre.y *rs +cercles[i*rs+num_col].centre.x), cercles[i*rs+num_col].radius_square);
			}

			if(heap_is_empty(h)==FALSE)
			{
				copy=heap_copy(h, heap_getsize(h));

				if(tab_heaps[i*rs+num_col]==NULL)
				{
					tab_heaps[i*rs+num_col]=copy;
				}
				else
				{
					copy2=heap_fusion(copy, tab_heaps[i*rs+num_col], heap_getsize(copy)+heap_getsize(tab_heaps[i*rs+num_col]));
					heap_free(copy);
					heap_free(tab_heaps[i*rs+num_col]);
					tab_heaps[i*rs+num_col]=copy2;
				}
			}

			cercles[i*rs+num_col]=max;
		}
	}

	heap_free(h);
}



void dilate_one_column_top(circle *cercles, uint32_t num_col, uint32_t rs, uint32_t cs, heap **tab_heaps)
{
	circle max, newmax;
	heap_element e;
	int32_t i,j;
	heap *h, *temp, *copy, *copy2;
	point p;

	h=heap_new(cs);
	max.radius_square=0;

	for(i=cs-1; i>=0; i--)
	{
		fprintf(stdout, "%d\n", heap_getsize(h));
		if(tab_heaps[i*rs+num_col]!=NULL)
		{
			temp=heap_fusion(h, tab_heaps[i*rs+num_col], h->size_max+heap_getsize(tab_heaps[i*rs+num_col]));
			heap_free(h);
			heap_free(tab_heaps[i*rs+num_col]);
			tab_heaps[i*rs+num_col]=NULL;
			h=temp;

			e=heap_remove_max(h);
			if(e.value > max.radius_square)
			{
				newmax.radius_square=e.value;
				newmax.centre.x=((uint32_t)e.element)%rs;
				newmax.centre.y=((uint32_t)e.element)/rs;;
				heap_add(h, (void*)(max.centre.y*rs + max.centre.x), max.radius_square);
				max=newmax;
			}
			else
			{
				heap_add(h, e.element, e.value);
			}
		}




		if(cercles[i*rs+num_col].radius_square>0)
		{
			if (max.radius_square!=0)
			{
				p.x=num_col;
				p.y=i;

				while(heap_is_empty(h)==FALSE && is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
				{
					e=heap_remove_max(h);
					max.radius_square=e.value;
					max.centre.x=((uint32_t)e.element)%rs;
					max.centre.y=((uint32_t)e.element)/rs;
				}

				if(is_touched_by_ball(p, max.centre, max.radius_square) == FALSE)
					max.radius_square=0;
			}

			if(max.radius_square==0)
			{
				max=cercles[i*rs+num_col];
			}
			else if(max.radius_square!=0 && cercles[i*rs+num_col].radius_square>=max.radius_square)
			{
				heap_add(h, (void*)(max.centre.y *rs +max.centre.x), max.radius_square);
				max=cercles[i*rs+num_col];
			}
			else
			{
				heap_add(h, (void*)(cercles[i*rs+num_col].centre.y *rs +cercles[i*rs+num_col].centre.x), cercles[i*rs+num_col].radius_square);
			}

			/*if(heap_is_empty(h)==FALSE)
			{
				copy=heap_copy(h, heap_getsize(h));

				if(tab_heaps[i*rs+num_col]==NULL)
				{
					tab_heaps[i*rs+num_col]=copy;
				}
				else
				{
					copy2=heap_fusion(copy, tab_heaps[i*rs+num_col], heap_getsize(copy)+heap_getsize(tab_heaps[i*rs+num_col]));
					heap_free(copy);
					heap_free(tab_heaps[i*rs+num_col]);
					tab_heaps[i*rs+num_col]=copy2;
				}
			}*/

			cercles[i*rs+num_col]=max;
		}
	}

	heap_free(h);
}







int main(int argc, char *argv[])
{
	struct xvimage *distmap, *new_distmap, *distmap_save, *output, *check;
	uint32_t rs, cs, d, i, j, n;
	point c;
	circle *cercles;
	distmap=readimage(argv[1]);
	heap **tab1, *h;


	rs=rowsize(distmap);
	cs=colsize(distmap);

	//new_distmap=allocimage(NULL, rs, cs, 1, VFF_TYP_4_BYTE);

	tab1=calloc(rs*cs, sizeof(heap*));
	//tab2=calloc(rs*cs, sizeof(heap*));

	cercles=calloc(rs*cs, sizeof(circle));
	n=0;
	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			cercles[n].centre.x=i;
			cercles[n].centre.y=j;
			cercles[n].radius_square=ULONGDATA(distmap)[n];
			n++;
		}

	//Algo naif de dilatation
	/*output=allocimage(NULL,rs, cs, 1, VFF_TYP_4_BYTE);
	n=0;
	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			if(ULONGDATA(distmap)[n]>0)
			{
				c.x=i; c.y=j;
				draw_a_baball(output, distmap, c, ULONGDATA(distmap)[n]);
			}
			n++;
		}
	writeimage(output, "sortie.pgm");*/


	/*for(i=0; i<rs; i++)
	{
		fprintf(stdout, "%3d ", ULONGDATA(distmap)[27*rs+i]);
	}

	fprintf(stdout, "\n\n\n");

	dilate_one_line_right(distmap, 27, rs);

	for(i=0; i<rs; i++)
	{
		fprintf(stdout, "%3d ", ULONGDATA(distmap)[27*rs+i]);
	}

	dilate_one_line_left(distmap, 27, rs);

	fprintf(stdout, "\n\n\n");

	for(i=0; i<rs; i++)
	{
		fprintf(stdout, "%3d ", ULONGDATA(distmap)[27*rs+i]);
	}

	return(0);*/


	for(j=0; j<cs; j++)
	{
		dilate_one_line_right(cercles, j, rs, tab1);
		dilate_one_line_left(cercles, j, rs, tab1);
	}

	/*n=0;
	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			h=heap_fusion(tab1[n], tab2[n], ULONGDATA(distmap));
			heap_destroy(tab1[n]);
			heap_destroy(tab2[n]);
			tab1[n]=h;
		}

	free(tab2);*/


	/*for(i=0; i<cs; i++)
	{
		fprintf(stdout, "%3d ", ULONGDATA(distmap)[i*rs+33]);
	}

	fprintf(stdout, "\n\n\n");

	dilate_one_column_bottom(distmap, 33, rs, cs);

	for(i=0; i<cs; i++)
	{
		fprintf(stdout, "%3d ", ULONGDATA(distmap)[i*rs+33]);
	}*/

	for(i=0; i<rs; i++)
	{
		dilate_one_column_bottom(cercles, i, rs, cs, tab1);
		dilate_one_column_top(cercles, i, rs, cs, tab1);
		fprintf(stdout, "%d\n", i);
	}

	n=0;
	for(j=0; j<cs; j++)
		for(i=0; i<rs; i++)
		{
			ULONGDATA(distmap)[n]=cercles[n].radius_square;
			n++;
		}


	writeimage(distmap, "sortie3.pgm");

	return(0);
}






