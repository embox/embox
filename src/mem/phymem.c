/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(phymem_init);

struct page_allocator *__phymem_allocator;

static int phymem_init(void) {
	char *const phymem_alloc_start = phymem_allocated_start();
	char *const phymem_alloc_end = phymem_allocated_end();
	const size_t mem_len = phymem_alloc_end - phymem_alloc_start;

	__phymem_allocator = page_allocator_init(phymem_alloc_start, mem_len, PAGE_SIZE());

	return 0;
}

char * const phymem_allocated_start(void) {
	extern char _reserve_end;

	return (char *) binalign_bound((uintptr_t) &_reserve_end, PAGE_SIZE());
}

char *const phymem_allocated_end(void) {
	extern char _ram_base;
	extern char _ram_size;

	return (char *)
			binalign_bound((uintptr_t) &_ram_base + (size_t) &_ram_size, PAGE_SIZE());
}

void *phymem_alloc(size_t page_number) {
	return page_alloc(__phymem_allocator, page_number);
}

void phymem_free(void *page, size_t page_number) {
	page_free(__phymem_allocator, page, page_number);
}

struct page_allocator *phy_allocator(void) {
	return __phymem_allocator;
}
