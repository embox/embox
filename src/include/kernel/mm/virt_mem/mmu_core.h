#include <hal/mm/mmu_core.h>

typedef void (*mmu_page_table_set_t)(mmu_pte_t * ptd, mmu_pte_t * pte);
typedef mmu_pmd_t* (*mmu_page_table_get_t)(mmu_pte_t ptd);

static unsigned long mmu_page_table_sizes[] = {
	0,
	MMU_GTABLE_SIZE * sizeof(mmu_pte_t),
	MMU_MTABLE_SIZE * sizeof(mmu_pte_t),
	MMU_PTABLE_SIZE * sizeof(mmu_pte_t),
	0
};

static unsigned long mmu_table_masks[] = {
	-1,
	MMU_GTABLE_MASK, /* warning: integer overflow in expression */
	MMU_MTABLE_MASK,
	MMU_PTABLE_MASK,
	MMU_PAGE_MASK,
	0
};

static unsigned long mmu_level_capacity[] = {
	-1,
	MMU_MTABLE_SIZE * MMU_PTABLE_SIZE * MMU_PAGE_SIZE,
	MMU_PTABLE_SIZE * MMU_PAGE_SIZE,
	MMU_PAGE_SIZE,
	1
};

static mmu_page_table_set_t mmu_page_table_sets[] = {
	NULL,
	&mmu_pgd_set,
	&mmu_pmd_set,
	&mmu_set_pte, /* warning: initialization from incompatible pointer type */
	NULL
};

static mmu_page_table_get_t mmu_page_table_gets[] = {
	NULL,
	&mmu_pgd_get, /* warning: initialization from incompatible pointer type */
	&mmu_pmd_get,
	&mmu_pmd_get,
	NULL
};
