/**
 * @file
 * @brief Test slab_dm allocator
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 */

#include <stdio.h>
#include <embox/test.h>
#include <lib/list.h>
#include <kernel/mm/slab_dm.h>

EMBOX_TEST(run)
;

#define ADDR_OF_MAIN_CACHE 0x40000574

/**
 * function to free cache list
 * @return 0 on success
 */
static int destroy_all_caches() {
	kmem_cache_t *kmalloc_cache = ADDR_OF_MAIN_CACHE;
	kmem_cache_t *cachep;

	while (1) {
		cachep = (kmem_cache_t*) kmalloc_cache->next.next;
		if (&cachep->next == &kmalloc_cache->next)
			break;
		kmem_dmcache_destroy(cachep);
	}

	return 0;
}
/**
 * testing smalloc() and sfree()
 * @return 0 on success
 */
static int test_s_functions() {
	kmem_cache_t *kmalloc_cache = ADDR_OF_MAIN_CACHE;
	void* objp[30];
	struct list_head *tmp;
	int i;

	for (i = 0; i < 10; i++) {
		objp[i] = smalloc(8);
	}
	for (i = 10; i < 20; i++) {
		objp[i] = smalloc(16);
	}
	for (i = 20; i < 30; i++) {
		objp[i] = smalloc(32);
	}
	/*how many caches are in cache list*/
	i = 0;
	list_for_each(tmp,&(kmalloc_cache->next)) {
		i++;
	}
	TRACE("\n%d caches were created for 3 types\n", i);

	for (i = 0; i < 30; i++) {
		sfree(objp[i]);
	}
	destroy_all_caches();
	i = 0;
	list_for_each(tmp,&(kmalloc_cache->next)) {
		i++;
	}
	TRACE("%d caches now\n", i);

	return 0;
}

static void test1() {
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
}

static void test2() {
	kmem_cache_t *cachep;

	for (int i = 0; i < 14; i++) {
		cachep = kmem_dmcache_create("cache1", 1 << i);
		kmem_dmcache_destroy(cachep);
	}
}

static int run(void) {
	int result = 0;
	if (0 != test_s_functions()) {
		TRACE("smalloc test FAILED\n");
		return -1;
	}
	test1();
	test2();

	return result;
}
