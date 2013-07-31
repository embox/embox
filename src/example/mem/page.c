/**
 * @file
 *
 * @brief This example explains using page allocation API.
 *
 * @details Page allocation API is low level memory allocator. It allocates
 *        physical page of the memory. It can be used for higher level
 *        allocators (malloc, pools etc.). You can reserve one or more physical
 *        page in it. A page size you can get with macro PAGE_SIZE(). This is
 *        special allocator and you must keep size of allocated segments
 *        yourself.
 *
 * @date 17.11.2011
 * @author Anton Bondarev
 */

#include <framework/example/self.h>
#include <mem/page.h>
#include <mem/phymem.h>

EMBOX_EXAMPLE(run);

static int run(int argc, char **argv) {
	void *page;
	void *pool;

	page = page_alloc(__phymem_allocator, 1);
	pool = page_alloc(__phymem_allocator, 10);
	page_free(__phymem_allocator, pool, 10);
	page_free(__phymem_allocator, page, 1);

	return 0;
}
