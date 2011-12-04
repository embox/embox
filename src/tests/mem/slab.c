/**
 * @file
 * @brief Test slab_dm allocator
 *
 * @date 22.12.10
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 */

#include <stdio.h>
#include <embox/test.h>
#include <lib/list.h>
#include <mem/misc/slab.h>

EMBOX_TEST_SUITE("mem/slab tests");

TEST_CASE("Create cache and allocate several object in it") {
	void* objp[10];
	size_t i;
	cache_t *cachep = cache_create("cache1", 50, 0);
	test_assert_not_null(cachep);

	for (i = 0; i < 10; i++) {
		objp[i] = cache_alloc(cachep);
		test_assert_not_null(objp[i]);

	}

	for (i = 0; i < 10; i++) {
		cache_free(cachep, objp[i]);
	}

	cache_destroy(cachep);

	cache_shrink(cachep);
}

TEST_CASE("Create several caches with different size and name") {
	cache_t *cachep;
	char cache_name[0x10];

	for (int i = 0; i < 14; i++) {
		sprintf(cache_name, "cache_%d", 1 << i);
		cachep = cache_create("cache_name", 1 << i, 0);
		test_assert_not_null(cachep);
		cache_destroy(cachep);
	}
}
