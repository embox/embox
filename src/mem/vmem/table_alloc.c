/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#include <assert.h>
#include <mem/vmem/table_alloc.h>
#include <hal/mm/mmu_core.h>

#define COUNT 100

static mmu_pte_t pte_table[COUNT][0x400] __attribute__((aligned(MMU_PAGE_SIZE)));
static int pte_c = 0;

static mmu_pmd_t pmd_table[COUNT][0x400] __attribute__((aligned(MMU_PAGE_SIZE)));
static int pmd_c = 0;

mmu_pte_t *alloc_pte_table() {
	assert(pte_c < COUNT);
	return (mmu_pte_t *) pte_table[pte_c++];
}

mmu_pmd_t *alloc_pmd_table() {
	assert(pmd_c < COUNT);
	return (mmu_pmd_t *) pmd_table[pmd_c++];
}

