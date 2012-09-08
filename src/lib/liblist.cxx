//Les bibliothèques standardes
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//Bibliothèque perso
#include <liblist.h>




cell* list_add_cell_to_head(list *actual_list)
{
	cell *actual_cell=list_getfreecell(actual_list);
	if(actual_cell==NULL)
	{
		fprintf(stderr, "list_add_cell_to_head(): Error when gathering new free cell for list.\n");
		return(NULL);
	}

	actual_cell->next=actual_list->head_data;
	actual_list->head_data=actual_cell;
	if(actual_list->end_data == NULL)
		actual_list->end_data = actual_list->head_data;

	actual_list->size ++;

	return(actual_cell);
}


cell* list_add_cell_to_queue(list *actual_list)
{
	cell *new_end=list_getfreecell(actual_list);
	if(new_end==NULL)
	{
		fprintf(stderr, "list_add_to_queue(): Error when gathering new free cell for list.\n");
		return(NULL);
	}

	if(actual_list->end_data != NULL)
		actual_list->end_data->next=new_end;
	else
		actual_list->head_data=new_end;

	actual_list->end_data=new_end;

	actual_list->size ++;

	return(new_end);
}


void list_delete(list *actual_list, char mode)
{
	cell *iter, *d;

	if(mode==FREE_DATA && (actual_list->mode==MODE_FIFO || actual_list->mode==MODE_LIFO ) )
	{
		list_free_all(actual_list, mode);
	}

	iter=actual_list->head_tab;
	while(iter!=NULL)
	{
		d=iter;
		iter=iter->next;
		free(d);
	}

	if(actual_list->mode==MODE_FIFO_DYN || actual_list->mode==MODE_LIFO_DYN)
	{
		free(actual_list->ret);
	}

	free(actual_list);
}


cell* list_gonext(cell *actual_cell)
{
	return (actual_cell->next);
}


int32_t list_hasnext(cell *actual_cell)
{
	return(actual_cell->next != NULL);
}


int32_t list_isempty(list* actual_list)
{
	return(actual_list->size==0);
}


cell *list_gethead(list *actual_list)
{
	return(actual_list->head_data);
}


uint32_t list_getsize(list* actual_list)
{
	return actual_list->size;
}


void list_free_all(list *actual_list, char mode)
{
	if(mode==MODE_FIFO_DYN || mode==MODE_LIFO_DYN)
	{
		cell *iter = actual_list->head_data;
		while (iter != NULL)
		{
			free(iter->data.data_pointer);
			iter=iter->next;
		}
	}
	else
	{
		fprintf(stderr, "list_free_all(): Cannot be used for non dynamic lists.\n");
	}
}

void list_make_empty(list *actual_list, char mode)
{
	if(actual_list->size > 0)
	{
		if(mode==FREE_DATA && (actual_list->mode==MODE_FIFO || actual_list->mode==MODE_LIFO ) )
		{
			list_free_all(actual_list, mode);
		}

		actual_list->end_data->next = actual_list->head_free;
		actual_list->head_free = actual_list->head_data;
		actual_list->head_data=NULL;
		actual_list->end_data=NULL;

		actual_list->size=0;
	}
}


//Source list is empty after this operation (not even free cells)
int32_t list_append_to_other_list(list *source_list, list *destination_list)
{
	if(source_list->mode != destination_list->mode || source_list->size_elem != destination_list->size_elem)
	{
		fprintf(stdout, "list_append_to_other_list(): Error, asked to append two lists which do not have same mode or same elements.\n");
		return(-1);
	}

	if(destination_list->size!=0)
		destination_list->end_data->next = source_list->head_data;
	else
		destination_list->head_data = source_list->head_data;
	destination_list->end_data = source_list->end_data;


	if(destination_list->end_free!=NULL)
		destination_list->end_free->next = source_list->head_free;
	else
		destination_list->head_free = source_list->head_free;
	destination_list->end_free = source_list->end_free;


	if(destination_list->end_tab!=NULL)
		destination_list->end_tab->next = source_list->head_tab;
	else
		destination_list->head_tab = source_list->head_tab;
	destination_list->end_tab = source_list->end_tab;


	source_list->head_data=NULL;
	source_list->end_data=NULL;
	source_list->head_free=NULL;
	source_list->end_free=NULL;
	source_list->head_tab=NULL;
	source_list->end_tab=NULL;

	destination_list->size += source_list->size;
	destination_list->capacity += source_list->capacity;

	source_list->size=0;
	source_list->capacity=0;

	return(0);
}


//ok
//If list is dynamic, data must be a pointer to data
//Else, it can be the data directly (which can be a pointer)
cell* list_getfreecell(list *actual_list)
{
	cell *result;

	if(actual_list->size == actual_list->capacity)
	{
		if( list_increase_capacity(actual_list, actual_list->capacity +1)!=0)
		{
			fprintf(stderr, "list_newcell(): Error while allocating memory.\n");
			return(NULL);
		}
	}

	result = actual_list->head_free;
	actual_list->head_free = actual_list->head_free -> next;
	if(actual_list->head_free == NULL)
		actual_list->end_free=NULL;
	result->next=NULL;

	return(result);
}


//ok
uint32_t list_increase_capacity(list *actual_list, uint32_t more_capacity)
{
	uint32_t i;
	cell *p, *pn;
	cell *tab = (cell*)calloc(more_capacity+1, sizeof(cell)+actual_list->size_elem-sizeof(void*));
	if(tab==NULL)
	{
		fprintf(stderr, "list_increase_capacity() : Memory allocation error.\n");
		return(1);
	}

	if(more_capacity > 0)
	{
		p=(cell*)((char*)tab+sizeof(cell)+actual_list->size_elem-sizeof(void*));
		pn=(cell*)((char*)p+sizeof(cell)+actual_list->size_elem-sizeof(void*));
		if(actual_list->size == actual_list->capacity) //no more free space
		{
			actual_list->head_free=p;
		}
		else
		{
			actual_list->end_free->next=p;
		}


		for(i=0; i<more_capacity-1; i++)
		{
			p->next=pn;
			p=pn;
			pn=(cell*)((char*)p+sizeof(cell)+actual_list->size_elem-sizeof(void*));
		}
		p->next=NULL;
		actual_list->end_free=p;
	}

	if(actual_list->head_tab==NULL)
		actual_list->head_tab=tab;
	else
		actual_list->end_tab->next = tab;
	actual_list->end_tab = tab;
	actual_list->end_tab->next=NULL;

	actual_list->capacity+=more_capacity;

	return(0);
}


//ok
list *list_newlist(char mode, uint32_t initial_capacity)
{
	if(mode==MODE_FIFO_DYN || mode==MODE_LIFO_DYN)
	{
		fprintf(stderr, "list_newlist(): If using dynamic list, please use list_newlist_dynam for creation.\n");
		return(NULL);
	}

	if(mode!=MODE_FIFO && mode!=MODE_LIFO)
	{
		fprintf(stderr, "list_newlist(): Wrong mode given.\n");
		return(NULL);
	}


	list *result = (list*)calloc(1, sizeof(list));
	if(result==NULL)
	{
		fprintf(stderr, "list_newlist(): Error while allocating memory.\n");
		return(NULL);
	}

	result->mode=mode;
	result->size_elem=sizeof(void*);


	if(list_increase_capacity(result, initial_capacity) != 0)
	{
		fprintf(stderr, "list_newlist(): Error while allocating memory (2).\n");
		return(NULL);
	}

	return result;
}


//ok
list *list_newlist_dynam(char mode, uint32_t size_elem, uint32_t initial_capacity)
{
	if(mode==MODE_FIFO || mode==MODE_LIFO)
	{
		fprintf(stdout, "list_newlist_dynam(): If using non dynamic list, please use list_newlist for creation.\n");
		return(NULL);
	}

	if(mode!=MODE_FIFO_DYN && mode!=MODE_LIFO_DYN)
	{
		fprintf(stderr, "list_newlist_dynam(): Wrong mode given.\n");
		return(NULL);
	}


	list *result = (list*)calloc(1, sizeof(list));
	if(result==NULL)
	{
		fprintf(stderr, "list_newlist_dynam(): Error while allocating memory.\n");
		return(NULL);
	}

	result->mode=mode;
	result->size_elem=size_elem;

	if(list_increase_capacity(result, initial_capacity) != 0)
	{
		fprintf(stderr, "list_newlist(): Error while allocating memory (2).\n");
		return(NULL);
	}

	result->ret = calloc(1, result->size_elem);
	if(result->ret==NULL)
	{
		fprintf(stderr, "list_newlist(): Error while allocating memory (3).\n");
		return(NULL);
	}

	return result;
}



//For classic list, gives directly back the data
//For dynamic list, gives a pointer on the element which is static memory allocation
//which means that a second call to list_pop will destroy data of the first call... so make backup !
void list_remove_head(list *actual_list)
{
	//No removal if no cell
	if(actual_list->head_data != NULL)
	{
		//And we "remove the cell" by placing it in the free cells list
		if(actual_list->end_free != NULL)
			actual_list->end_free->next = actual_list->head_data;
		else
			actual_list->head_free = actual_list->head_data;
		actual_list->end_free = actual_list->head_data;
		actual_list->head_data = actual_list->head_data->next;
		actual_list->end_free->next=NULL;

		if(actual_list->head_data == NULL)
			actual_list->end_data = NULL;

		actual_list->size --;
	}
}


uint8_t list_pop_uint8_t(list *actual_list)
{
	if(actual_list->head_data != NULL)
	{
		uint8_t ret = actual_list->head_data->data.data_uint8_t;
		list_remove_head(actual_list);
		return ret;
	}
	else
	{
		fprintf(stderr, "list_pop_uint8_t(): Warning, pop function called but list is empty.\n");
		return 0;
	}
}


uint16_t list_pop_uint16_t(list *actual_list)
{
	if(actual_list->head_data != NULL)
	{
		uint16_t ret = actual_list->head_data->data.data_uint16_t;
		list_remove_head(actual_list);
		return ret;
	}
	else
	{
		fprintf(stderr, "list_pop_uint16_t(): Warning, pop function called but list is empty.\n");
		return 0;
	}
}


uint32_t list_pop_uint32_t(list *actual_list)
{
	if(actual_list->head_data != NULL)
	{
		uint32_t ret = actual_list->head_data->data.data_uint32_t;
		list_remove_head(actual_list);
		return ret;
	}
	else
	{
		fprintf(stderr, "list_pop_uint32_t(): Warning, pop function called but list is empty.\n");
		return 0;
	}
}


double list_pop_double(list *actual_list)
{
	if(actual_list->head_data != NULL)
	{
		double ret = actual_list->head_data->data.data_double;
		list_remove_head(actual_list);
		return ret;
	}
	else
	{
		fprintf(stderr, "list_pop_double(): Warning, pop function called but list is empty.\n");
		return 0.0;
	}
}


void* list_pop_pointer(list *actual_list)
{
	if(actual_list->head_data != NULL)
	{
		void *ret=NULL;

		if(actual_list->mode==MODE_LIFO || actual_list->mode==MODE_FIFO)
		{
			//We pop, so we remove the head pointer... and give back the data (so we don't free the data)
			ret=actual_list->head_data->data.data_pointer;
		}
		else if(actual_list->mode==MODE_LIFO_DYN || actual_list->mode==MODE_FIFO_DYN)
		{
			memcpy(actual_list->ret, &(actual_list->head_data->data.data_pointer), actual_list->size_elem);
			ret=actual_list->ret;
		}
		list_remove_head(actual_list);
		return ret;
	}
	else
	{
		fprintf(stderr, "list_pop_pointer(): Warning, pop function called but list is empty.\n");
		return NULL;
	}
}


void list_push_uint8_t(list *actual_list, uint8_t data)
{
	cell* a = list_push_cell(actual_list);
	assert(a!=NULL);
	a->data.data_uint8_t=data;
}


void list_push_uint16_t(list *actual_list, uint16_t data)
{
	cell* a = list_push_cell(actual_list);
	assert(a!=NULL);
	a->data.data_uint16_t=data;
}


void list_push_uint32_t(list *actual_list, uint32_t data)
{
	cell* a = list_push_cell(actual_list);
	assert(a!=NULL);
	a->data.data_uint32_t=data;
}


void list_push_double(list *actual_list, double data)
{
	cell* a = list_push_cell(actual_list);
	assert(a!=NULL);
	a->data.data_double=data;
}


//If list is not dynamic, then data must be directly the data to push
//If list is dynamic, data must be a pointer to the element to push
void list_push_pointer(list *actual_list, void* data)
{
	cell* a = list_push_cell(actual_list);
	assert(a!=NULL);

	if( actual_list->mode==MODE_FIFO_DYN || actual_list->mode==MODE_LIFO_DYN)
		memcpy(&(a->data.data_pointer), data, actual_list->size_elem);
	else
		a->data.data_pointer=data;
}


cell* list_push_cell(list *actual_list)
{
	if(actual_list->mode==MODE_FIFO || actual_list->mode==MODE_FIFO_DYN)
	{
		return list_add_cell_to_queue(actual_list);
	}
	else if(actual_list->mode==MODE_LIFO || actual_list->mode==MODE_LIFO_DYN)
	{
		return list_add_cell_to_head(actual_list);
	}
	return NULL;
}


uint8_t list_read_uint8_t(list *actual_list, cell *actual_cell)
{
	return actual_cell->data.data_uint8_t;
}


uint16_t list_read_uint16_t(list *actual_list, cell *actual_cell)
{
	return actual_cell->data.data_uint16_t;
}


uint32_t list_read_uint32_t(list *actual_list, cell *actual_cell)
{
	return actual_cell->data.data_uint32_t;
}


double list_read_double(list *actual_list, cell *actual_cell)
{
	return actual_cell->data.data_double;
}


//For classic list, gives directly back the data
//For dynamic list, gives a pointer on the element
void* list_read_pointer(list *actual_list, cell *actual_cell)
{
	if(actual_list->mode==MODE_LIFO_DYN || actual_list->mode==MODE_FIFO_DYN)
		return(&(actual_cell->data.data_pointer));
	else
		return(actual_cell->data.data_pointer);
}

