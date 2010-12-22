/**
 * @file
 * @brief Test slab_dm allocator
 * @author Kirill Tyushev
 */

#include <stdio.h>

#include <embox/test.h>
#include <kernel/mm/slab_dm.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */

static int run(void) {
	int result = 0;
	void* objp[10];

	kmem_cache_t *cachep = kmem_dmcache_create("cache1", 50);

	for (int i = 0; i < 10; i++) {
		objp[i] = kmem_dmcache_alloc(cachep);
	}

	for (int i = 0; i < 10; i++) {
		kmem_dmcache_free(cachep, objp[i]);
	}

	kmem_dmcache_destroy(cachep);

	kmem_dmcache_shrink(cachep);

	return result;
}

