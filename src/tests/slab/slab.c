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
#include <kernel/mm/slab.h>
#include <kernel/mm/kmalloc.h>

EMBOX_TEST(run);

/**
 * testing kmalloc() and kfree()
 * @return 0 on success
 */
static int test_s_functions(void) {
	void* objp[30];
	size_t i;

	for (i = 0; i < 10; i++) {
		objp[i] = kmalloc(8);
	}
	for (i = 10; i < 20; i++) {
		objp[i] = kmalloc(16);
	}
	for (i = 20; i < 30; i++) {
		objp[i] = kmalloc(32);
	}

	for (i = 0; i < 30; i++) {
		kfree(objp[i]);
	}

	return 0;
}

static void test1(void) {
	void* objp[10];
	size_t i;
	cache_t *cachep = cache_create("cache1", 50);

	for (i = 0; i < 10; i++) {
		objp[i] = cache_alloc(cachep);
	}

	for (i = 0; i < 10; i++) {
		cache_free(cachep, objp[i]);
	}

	cache_destroy(cachep);

	cache_shrink(cachep);
}

static void test2(void) {
	cache_t *cachep;

	for (int i = 0; i < 14; i++) {
		cachep = cache_create("cache1", 1 << i);
		cache_destroy(cachep);
	}
}

static int run(void) {
	if (0 != test_s_functions()) {
		TRACE("kmalloc test FAILED\n");
		return -1;
	}
	test1();
	test2();

	return 0;
}
