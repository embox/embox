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

#include <embox/unit.h>
#include <mem/vmem/vmem_alloc.h>
#include <util/binalign.h>

#define VIRTUAL_TABLES_COUNT  OPTION_GET(NUMBER, virtual_tables_count)
#define VIRTUAL_PAGES_COUNT   OPTION_GET(NUMBER, virtual_pages_count)

static char *virtual_tables;
static char *virtual_page_info;

static char virtual_tables_raw[VIRTUAL_TABLES_COUNT + 1][MMU_PAGE_SIZE];
static struct page_allocator *virt_table_allocator;
static char virtual_page_info_raw[VIRTUAL_PAGES_COUNT + 1][MMU_PAGE_SIZE];
static struct page_allocator *virt_page_allocator;

EMBOX_UNIT_INIT(vmem_alloc_init);

static int vmem_alloc_init(void) {
	virtual_tables = (char*) binalign_bound((uintptr_t) virtual_tables_raw, MMU_PAGE_SIZE);
	virtual_page_info = (char*) binalign_bound((uintptr_t) virtual_page_info_raw, MMU_PAGE_SIZE);

	virt_table_allocator = page_allocator_init(virtual_tables, VIRTUAL_TABLES_COUNT * MMU_PAGE_SIZE, MMU_PAGE_SIZE);
	virt_page_allocator = page_allocator_init(virtual_page_info, VIRTUAL_PAGES_COUNT * MMU_PAGE_SIZE, MMU_PAGE_SIZE);
	return 0;
}

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
