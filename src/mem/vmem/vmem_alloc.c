/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#include <util/log.h>
#include <framework/mod/options.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <hal/mmu.h>
#include <mem/page.h>

#include <embox/unit.h>
#include <mem/vmem/vmem_alloc.h>
#include <util/binalign.h>

#define PGD_COUNT  OPTION_GET(NUMBER, pgd_count)
#define PMD_COUNT  OPTION_GET(NUMBER, pmd_count)
#define PTE_COUNT  OPTION_GET(NUMBER, pte_count)

#define PGD_ALIGN  OPTION_GET(NUMBER, pgd_align)
#define PMD_ALIGN  OPTION_GET(NUMBER, pmd_align)
#define PTE_ALIGN  OPTION_GET(NUMBER, pte_align)

#if PGD_ALIGN != 0
#undef  MMU_PGD_SIZE
#define MMU_PGD_SIZE PGD_ALIGN
#endif

#if PMD_ALIGN != 0
#undef  MMU_PMD_SIZE
#define MMU_PMD_SIZE PMD_ALIGN
#endif

#if PTE_ALIGN != 0
#undef  MMU_PTE_SIZE
#define MMU_PTE_SIZE PTE_ALIGN
#endif

#define VIRTUAL_PAGES_COUNT   OPTION_GET(NUMBER, virtual_pages_count)

static char pgd_raw[(PGD_COUNT + 1) * MMU_PGD_SIZE] __attribute__ ((aligned(MMU_PGD_SIZE)));
static char pmd_raw[(PMD_COUNT + 1) * MMU_PMD_SIZE] __attribute__ ((aligned(MMU_PMD_SIZE)));

static struct page_allocator *pgd_allocator;
static struct page_allocator *pmd_allocator;
#if (PTE_COUNT > 1)
static struct page_allocator *pte_allocator;
static char pte_raw[(PTE_COUNT + 1) * MMU_PTE_SIZE] __attribute__ ((aligned(MMU_PTE_SIZE)));
#endif

static char virtual_page_raw[(VIRTUAL_PAGES_COUNT + 1) * MMU_PAGE_SIZE] __attribute__ ((section(".bss.vmem_pages")));
static struct page_allocator *virt_page_allocator;

EMBOX_UNIT_INIT(vmem_alloc_init);

static int vmem_alloc_init(void) {
	if (pgd_allocator) {
		/* Already initialized */
		return 0;
	}

	pgd_allocator = page_allocator_init(pgd_raw, sizeof(pgd_raw), MMU_PGD_SIZE);
	pmd_allocator = page_allocator_init(pmd_raw, sizeof(pmd_raw), MMU_PMD_SIZE);
#if (PTE_COUNT > 1)
	pte_allocator = page_allocator_init(pte_raw, sizeof(pte_raw), MMU_PTE_SIZE);
	assert(pte_allocator);
#endif
	assert(pgd_allocator);
	assert(pmd_allocator);

	virt_page_allocator = page_allocator_init(virtual_page_raw, VIRTUAL_PAGES_COUNT * MMU_PAGE_SIZE, MMU_PAGE_SIZE);

	log_debug("%p", virt_page_allocator);
	return 0;
}

mmu_pgd_t *vmem_alloc_pgd_table(void) {
	void *addr;

	if (!pgd_allocator) {
		vmem_alloc_init();
	}

	addr = page_alloc(pgd_allocator, 1);

	if (addr)
		memset(addr, 0, MMU_PGD_SIZE);

	log_debug("%p", addr);

	return addr;
}

mmu_pmd_t *vmem_alloc_pmd_table(void) {
	void *addr;

	assert(pmd_allocator);

	addr = page_alloc(pmd_allocator, 1);

	if (addr)
		memset(addr, 0, MMU_PMD_SIZE);

//	log_debug("%p", addr);

	return addr;
}


mmu_pte_t *vmem_alloc_pte_table(void) {
#if (PTE_COUNT > 1)
	void *addr;

	assert(pte_allocator);

	addr = page_alloc(pte_allocator, 1);

	if (addr)
		memset(addr, 0, MMU_PTE_SIZE);

	log_debug("%p", addr);

	return addr;
#else
	log_error("PTE pool size is zero for current configuration");

	return NULL;
#endif
}

void *vmem_alloc_page() {
	assert(virt_page_allocator);

	return page_alloc(virt_page_allocator, 1);
}

/*
 * ------------------ Free ------------------
 */

void vmem_free_pgd_table(mmu_pgd_t *pgd) {
	page_free(pgd_allocator, pgd, 1);
}

void vmem_free_pmd_table(mmu_pmd_t *pmd) {
	page_free(pmd_allocator, pmd, 1);
}

void vmem_free_pte_table(mmu_pte_t *pte) {
#if (PTE_COUNT > 1)
	page_free(pte_allocator, pte, 1);
#endif
}

void vmem_free_page(void *addr) {
	if (page_belong(virt_page_allocator, addr))
		page_free(virt_page_allocator, addr, 1);
}

struct page_allocator *get_pgd_allocator(void) {
	return pgd_allocator;
}

struct page_allocator *get_pmd_allocator(void) {
	return pmd_allocator;
}

struct page_allocator *get_pte_allocator(void) {
#if (PTE_COUNT > 1)
	return pte_allocator;
#endif
	return NULL;
}
