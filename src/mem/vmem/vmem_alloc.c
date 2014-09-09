/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#include <framework/mod/options.h>

#include <errno.h>
#include <string.h>
#include <hal/mmu.h>
#include <mem/page.h>

#include <mem/vmem/vmem_alloc.h>

#define VIRTUAL_TABLES_COUNT  OPTION_GET(NUMBER, virtual_tables_count)
#define VIRTUAL_PAGES_COUNT   OPTION_GET(NUMBER, virtual_pages_count)

static char virtual_tables[VIRTUAL_TABLES_COUNT][MMU_PAGE_SIZE] __attribute__((aligned(MMU_PAGE_SIZE)));
PAGE_ALLOCATOR_DEF(static_table_allocator, virtual_tables, VIRTUAL_TABLES_COUNT, MMU_PAGE_SIZE);
static struct page_allocator *virt_table_allocator = &static_table_allocator;


static char virtual_page_info[VIRTUAL_PAGES_COUNT][MMU_PAGE_SIZE] __attribute__((aligned(MMU_PAGE_SIZE)));
PAGE_ALLOCATOR_DEF(static_page_info_allocator, virtual_page_info, VIRTUAL_PAGES_COUNT, MMU_PAGE_SIZE);
static struct page_allocator *virt_page_allocator = &static_page_info_allocator;


/*
 * ------------------ Alloc ------------------
 */

static inline void *vmem_alloc_table(void) {
	void *addr = page_alloc(virt_table_allocator, 1);
	if (addr) {
		memset(addr, 0 , MMU_PAGE_SIZE);
	}
	return addr;
}

mmu_pgd_t *vmem_alloc_pgd_table(void) {
	return (mmu_pgd_t *) vmem_alloc_table();
}

mmu_pmd_t *vmem_alloc_pmd_table(void) {
	return (mmu_pmd_t *) vmem_alloc_table();
}

mmu_pte_t *vmem_alloc_pte_table(void) {
	return (mmu_pte_t *) vmem_alloc_table();
}

void *vmem_alloc_page() {
	return page_alloc(virt_page_allocator, 1);
}

/*
 * ------------------ Free ------------------
 */

void vmem_free_pgd_table(mmu_pgd_t *pgd) {
	page_free(virt_table_allocator, pgd, 1);
}

void vmem_free_pmd_table(mmu_pmd_t *pmd) {
	page_free(virt_table_allocator, pmd, 1);
}

void vmem_free_pte_table(mmu_pte_t *pte) {
	page_free(virt_table_allocator, pte, 1);
}

void vmem_free_page(void *addr) {
	page_free(virt_page_allocator, addr, 1);
}
