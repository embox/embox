/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <errno.h>
#include <string.h>
#include <hal/mmu.h>
#include <mem/vmem/virtalloc.h>
#include <mem/phymem.h>
#include <mem/page.h>

EMBOX_UNIT(unit_init, unit_fini);

#define VIRTUAL_TABLES_COUNT (1000UL)
#define VIRTUAL_PAGES_COUNT  (100UL)

static struct page_allocator *virt_table_allocator;
static struct page_allocator *virt_page_allocator;

void *VIRTUAL_TABLES_START;
size_t VIRTUAL_TABLES_LEN;

void *VIRTUAL_PAGES_INFO_START;
size_t VIRTUAL_PAGES_INFO_LEN;

static int unit_init() {
	/* Initialize tables allocator. */
	VIRTUAL_TABLES_LEN = VIRTUAL_TABLES_COUNT * MMU_PAGE_SIZE;
	if (!(VIRTUAL_TABLES_START = page_alloc(
			__phymem_allocator,
			VIRTUAL_TABLES_LEN / PAGE_SIZE()))) {

		return -ENOMEM;
	}
	virt_table_allocator = page_allocator_init(
			VIRTUAL_TABLES_START,
			VIRTUAL_TABLES_LEN,
			MMU_PAGE_SIZE);

	/* Initialize pages allocator. */
	if (!(VIRTUAL_PAGES_INFO_START = page_alloc(
			__phymem_allocator,
			VIRTUAL_PAGES_COUNT * MMU_PAGE_SIZE / PAGE_SIZE()))) {

		return -ENOMEM;
	}
	virt_page_allocator = page_allocator_init(
			VIRTUAL_PAGES_INFO_START,
			__phymem_allocator->free,
			MMU_PAGE_SIZE);

	VIRTUAL_PAGES_INFO_LEN = virt_page_allocator->capacity - virt_page_allocator->free;

	return ENOERR;
}

static int unit_fini() {
	return 0;
}

void *virt_alloc_table() {
	void *addr = page_alloc(virt_table_allocator, 1);
	memset(addr, 0 , MMU_PAGE_SIZE);
	return addr;
}

void *virt_alloc_page() {
	return page_alloc(virt_page_allocator, 1);
}
