/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */

#include <embox/unit.h>

#include <stdint.h>

#include <kernel/printk.h>
#include <mem/phymem.h>
#include <mem/page.h>
#include <util/binalign.h>

EMBOX_UNIT_INIT(phymem_init);

struct page_allocator *__phymem_allocator;

static int phymem_init(void) {
	extern char _ram_base;
	extern char _ram_size;
	extern char _reserve_end;

	char *mem_start = (char *) binalign_bound((uintptr_t) &_reserve_end,
			PAGE_SIZE());
	char *mem_end = &_ram_base + (size_t) &_ram_size;
	size_t mem_len = mem_end - mem_start;

	printk("start=%p, end=%p, size=%zu\n", mem_start, mem_end, mem_len);

	__phymem_allocator = page_allocator_init(mem_start, mem_len, PAGE_SIZE());

	return 0;
}
