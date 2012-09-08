#ifndef _LIB_GENERAL_H_
#define _LIB_GENERAL_H_

#define getxfrompixnum(p,rs,ps) (p%rs)
#define getyfrompixnum(p,rs,ps) ((p%ps)/rs)
#define getzfrompixnum(p,rs,ps) (p/ps)

uint32_t set_size_element_for_generic_qsort(size_t size);
int generic_compare_for_qsort(const void *a, const void *b);
void generic_qsort(void *base, uint32_t num_elements, size_t size_elements);
uint32_t dichotomic_research_on_void(void* tab, void* look_for, uint32_t num_elements, uint32_t size_element);
uint32_t dichotomic_research_on_uint32t(uint32_t *tab, uint32_t lookfor, uint32_t num_elements);
uint32_t count_isolated_faces(uint32_t code_faces, uint32_t code_surf_edges);

#endif
