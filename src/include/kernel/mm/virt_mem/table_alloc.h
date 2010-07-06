#include <types.h>

extern unsigned long *mmu_table_alloc(size_t size);

extern void mmu_table_free(unsigned long *table, int level);
