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

	page = phymem_alloc(1);
	pool = phymem_alloc(10);
	phymem_free(pool, 10);
	phymem_free(page, 1);

	return 0;
}
