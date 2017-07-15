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

#define STMF7 1

#define ALLOCATOR_NAME OPTION_GET(STRING,allocator_name)

struct page_allocator *ALLOCATOR_NAME;

#if STMF7
#define HEAP_START 0xc0200000
#define HEAP_END   0xc0A00000

static int heap_init(void) {
	size_t heap_size = (size_t)HEAP_END - (size_t)HEAP_START;
	ALLOCATOR_NAME = page_allocator_init((char *)HEAP_START, heap_size, PAGE_SIZE());
	if (NULL == ALLOCATOR_NAME) {
		return -ENOSUPP;
	}

	return 0;
}

#else

#define HEAP_START OPTION_GET(STRING,start_symbol)
#define HEAP_END OPTION_GET(STRING,end_symbol)

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
#endif