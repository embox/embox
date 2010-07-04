#include <hal/mm/mmu_core.h>
#define DEBUG

typedef void (*mmu_page_table_set_t)(mmu_pte_t * ptd, mmu_pte_t * pte);
typedef mmu_pmd_t* (*mmu_page_table_get_t)(mmu_pte_t ptd);

static unsigned long mmu_page_table_sizes[] = {
	0,
	MMU_PGD_TABLE_SIZE,
	MMU_PMD_TABLE_SIZE,
	MMU_PTE_TABLE_SIZE
};

static unsigned long mmu_table_masks[] = {
	-1,
	MMU_GTABLE_MASK,
	MMU_MTABLE_MASK,
	MMU_PTABLE_MASK,
	MMU_PAGE_MASK,
	0
};

static unsigned long mmu_table_offsets[] = {
	0,
	MMU_GTABLE_MASK_OFFSET,
	MMU_MTABLE_MASK_OFFSET,
	MMU_PTABLE_MASK_OFFSET,
	0
};

static unsigned long mmu_level_capacity[] = {
	-1,
	MMU_MTABLE_MASK + (1<<MMU_MTABLE_MASK_OFFSET),
	MMU_PTABLE_MASK + (1<<MMU_PTABLE_MASK_OFFSET),
	MMU_PAGE_MASK + 1,
	1
};

static mmu_page_table_set_t mmu_page_table_sets[] = {
	NULL,
	&mmu_pgd_set,
	&mmu_pmd_set,
	&mmu_set_pte,
	NULL
};

static mmu_page_table_get_t mmu_page_table_gets[] = {
	NULL,
	&mmu_pgd_get,
	&mmu_pmd_get,
	&mmu_pmd_get,
	NULL
};

