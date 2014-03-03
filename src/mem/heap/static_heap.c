/**
 * @file
 * @brief Init page global allocator on static heap.
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */
#include <errno.h>

#include <embox/unit.h>
#include <mem/page.h>


EMBOX_UNIT_INIT(heap_init);

struct page_allocator *__heap_pgallocator;
extern char _heap_start;
extern char _heap_end;

static int heap_init(void) {
	size_t heap_size = (size_t)&_heap_end - (size_t)&_heap_start;

	__heap_pgallocator = page_allocator_init((char *)&_heap_start, heap_size, PAGE_SIZE());
	if (NULL == __heap_pgallocator) {
		return -ENOSUPP;
	}

	return 0;
}
