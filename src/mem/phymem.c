/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */
#include <embox/unit.h>

#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>

EMBOX_UNIT_INIT(phymem_init);

struct page_allocator *__phymem_allocator;
extern char _mem_begin;
extern char _mem_length;
extern char _heap_end;

static int phymem_init(void) {
	size_t free_start = binalign_bound((size_t)&_heap_end, PAGE_SIZE());
	size_t free_len = (size_t)&_mem_length - (free_start - (size_t)&_mem_begin);

	__phymem_allocator = page_allocator_init((char *)free_start, free_len, PAGE_SIZE());

	return 0;
}
