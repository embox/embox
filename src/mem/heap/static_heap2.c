/**
 * @file
 * @brief Full copy of static_heap.c
 *
 * @author  Anton Kozlov
 * @date    13.05.2015
 */

#include <errno.h>

#include <embox/unit.h>
#include <mem/page.h>

EMBOX_UNIT_INIT(heap_init);

#define HEAP_START OPTION_GET(STRING,start_symbol)
#define HEAP_END OPTION_GET(STRING,end_symbol)

#define ALLOCATOR_NAME OPTION_GET(STRING,allocator_name)

struct page_allocator *ALLOCATOR_NAME;
extern char HEAP_START;
extern char HEAP_END;

static int heap_init(void) {
	size_t heap_size = (size_t)&HEAP_END - (size_t)&HEAP_START;

	ALLOCATOR_NAME = page_allocator_init((char *)&HEAP_START, heap_size, PAGE_SIZE());
	if (NULL == ALLOCATOR_NAME) {
		return -ENOSUPP;
	}

	return 0;
}
