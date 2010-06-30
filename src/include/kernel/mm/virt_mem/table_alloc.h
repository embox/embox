#include <types.h>

/* it should be computed but seems too complicated (must be aligned
 * for _all_ possible requests for space -- this the main feature of
 * table_alloc) */
#define PAGE_HEADER_SIZE 1024

extern unsigned long *mmu_table_alloc(size_t size);

extern void mmu_table_free(unsigned long *table, int level);
