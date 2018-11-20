/**
 * @file
 *
 * @date 04.06.2017
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <errno.h>

#include <embox/unit.h>
#include <mem/page.h>

EMBOX_UNIT_INIT(fixed_heap_init);

#define HEAP_START  OPTION_GET(NUMBER,heap_start)
#define HEAP_SIZE   OPTION_GET(NUMBER,heap_size)

#define ALLOCATOR_NAME OPTION_GET(STRING,allocator_name)

struct page_allocator *ALLOCATOR_NAME;

static int fixed_heap_init(void) {
	ALLOCATOR_NAME = page_allocator_init((char *)HEAP_START, HEAP_SIZE, PAGE_SIZE());
	if (NULL == ALLOCATOR_NAME) {
		return -ENOSUPP;
	}

	return 0;
}
