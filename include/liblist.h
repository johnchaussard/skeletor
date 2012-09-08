#ifndef _LIB_LIST_H_
#define _LIB_LIST_H_

#include <stdint.h>

#define FREE_DATA 0
#define NO_FREE_DATA 1

#define MODE_FIFO 0
#define MODE_LIFO 1
#define MODE_FIFO_DYN 3
#define MODE_LIFO_DYN 4


typedef enum list_types
{
	typ_uint8_t,
	typ_uint16_t,
	typ_uint32_t,
	typ_double,
	typ_pointer
} list_types;

typedef union datatype
{
	uint8_t data_uint8_t;
	uint16_t data_uint16_t;
	uint32_t data_uint32_t;
	double data_double;
	void* data_pointer;
} datatype;

typedef struct cell
{
	struct cell *next;
	datatype data;
}cell;

typedef struct list
{
	char mode;

	cell *head_data;
	cell *end_data;

	cell *head_free;
	cell *end_free;

	cell *head_tab;
	cell *end_tab;

	uint32_t size;
	uint32_t capacity;

	uint32_t size_elem; //in sizeof(unsigned char), so in byte (size of data to store - sizeof(void*))
	void* ret; //This will be use by the pop function
}list;

typedef list jlist;


cell* list_add_cell_to_head(list *actual_list);
cell* list_add_cell_to_queue(list *actual_list);
void list_delete(list *actual_list, char mode);
cell* list_gonext(cell *actual_cell);
int32_t list_hasnext(cell *actual_cell);
int32_t list_isempty(list* actual_list);
cell *list_gethead(list *actual_list);
void list_free_all(list *actual_list, char mode);
void list_make_empty(list *actual_list, char mode);
int32_t list_append_to_other_list(list *source_list, list *destination_list);
cell* list_getfreecell(list *actual_list);
uint32_t list_increase_capacity(list *actual_list, uint32_t more_capacity);
list *list_newlist(char mode, uint32_t initial_capacity);
list *list_newlist_dynam(char mode, uint32_t size_elem, uint32_t initial_capacity);
void list_remove_head(list *actual_list);
uint8_t list_pop_uint8_t(list *actual_list);
uint16_t list_pop_uint16_t(list *actual_list);
uint32_t list_pop_uint32_t(list *actual_list);
double list_pop_double(list *actual_list);
void* list_pop_pointer(list *actual_list);
void list_push_uint8_t(list *actual_list, uint8_t data);
void list_push_uint16_t(list *actual_list, uint16_t data);
void list_push_uint32_t(list *actual_list, uint32_t data);
void list_push_double(list *actual_list, double data);
void list_push_pointer(list *actual_list, void* data);
cell* list_push_cell(list *actual_list);
uint8_t list_read_uint8_t(list *actual_list, cell *actual_cell);
uint16_t list_read_uint16_t(list *actual_list, cell *actual_cell);
uint32_t list_read_uint32_t(list *actual_list, cell *actual_cell);
double list_read_double(list *actual_list, cell *actual_cell);
void* list_read_pointer(list *actual_list, cell *actual_cell);
uint32_t list_getsize(list* actual_list);

#endif
