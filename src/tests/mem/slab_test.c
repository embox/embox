/**
 * @file
 * @brief Test slab allocator
 *
 * @date 22.12.10
 * @author Kirill Tyushev
 * @author Alexander Kalmuk
 */

#include <embox/test.h>
#include <mem/misc/slab.h>
#include <lib/libds/dlist.h>
#include <mem/page.h>

#if 0
static size_t list_length(struct dlist_head *head);
#endif
/* Used to fill slab with one object */
#define MAX_SIZE (PAGE_SIZE() - 64)

EMBOX_TEST_SUITE("slab allocator test");

struct big_object {
	char some_stuff[MAX_SIZE];
};
CACHE_DEF(static_cache, struct big_object , 0);

TEST_CASE("Allocation in cache created throw CACHE_DEF without cache growing.") {
#if 0
	void* obj;

	/* Test initial cache's property: no slabs */
	test_assert_equal(static_cache.slab_order, 0);
	test_assert(dlist_empty(&static_cache.slabs_free));
	test_assert(dlist_empty(&static_cache.slabs_partial));
	test_assert(dlist_empty(&static_cache.slabs_full));

	/* Allocate object. */
	obj = cache_alloc(&static_cache);
	test_assert_not_null(obj);
	test_assert(dlist_empty(&static_cache.slabs_free));
	test_assert_equal(list_length(&static_cache.slabs_full), 1);

	/* Free object. */
	cache_free(&static_cache, obj);
	test_assert_equal(list_length(&static_cache.slabs_free), 1);
	test_assert(dlist_empty(&static_cache.slabs_full));

	cache_destroy(&static_cache);
#endif
}

TEST_CASE("Allocation in multiple slabs with cache growing.") {
#if 0
	void *obj;
	cache_t *cache;

	cache = cache_create("test_cache", MAX_SIZE / 2, 0);

	/* Fill cache */
	test_assert_not_null(cache_alloc(cache));
	test_assert_not_null(cache_alloc(cache));
	test_assert_equal(list_length(&cache->slabs_full), 1);

	/* Test if third object will be allocated */
	obj = cache_alloc(cache);
	test_assert_not_null(obj);

	/* Test cache have one full slab with two objects and partial full slab with one object. */
	test_assert_equal(list_length(&cache->slabs_full), 1);
	test_assert_equal(list_length(&cache->slabs_partial), 1);

	/* Then free last allocated object. Test count of free slabs */
	cache_free(cache, obj);
	test_assert_equal(list_length(&cache->slabs_full), 1);
	test_assert_equal(list_length(&cache->slabs_free), 1);

	cache_destroy(cache);
#endif
}

TEST_CASE("Cache shrinking.") {
#if 0
	void *obj[2];
	cache_t *cache;

	cache = cache_create("test_cache", MAX_SIZE, 1);

	/* Allocate two objects. */
	test_assert_not_null(cache);
	test_assert_not_null((obj[0] = cache_alloc(cache)));
	test_assert_not_null((obj[1] = cache_alloc(cache)));
	test_assert_equal(list_length(&cache->slabs_full), 2);

	/* Free objects and shrink cache. */
	cache_free(cache, obj[0]);
	cache_free(cache, obj[1]);
	test_assert_equal(list_length(&cache->slabs_free), 2);
	cache_shrink(cache);
	test_assert(dlist_empty(&cache->slabs_free));

	cache_destroy(cache);
#endif
}

TEST_CASE("Slab size.") {
#if 0
	/* Slab allocater store objects in slabs with size 2^n. It is very
	 * effective when low level allocator uses buddy strategy. So, test if slab size is appropriate. */

	cache_t *cache;
	size_t num, slab_size;
	/* Object with "bad" size */
	size_t obj_size = MAX_SIZE - (MAX_SIZE / MAX_INT_FRAGM_ORDER);

	/* Create object with wastage greater than (1 / MAX_INT_FRAGM_ORDER) * 100% */
	cache = cache_create("test_cache", obj_size , 1);
	num = cache->num;
	while (num-- > 0) {
		cache_alloc(cache);
	}
	slab_size = PAGE_SIZE() << cache->slab_order;
	/* Test if full slab have wastage lower than (1 / MAX_INT_FRAGM_ORDER) * 100% or slab has
	 * maximum size */
	test_assert(((obj_size * cache->num) / slab_size) <= (1 / MAX_INT_FRAGM_ORDER) ||
			(MAX_SLAB_ORDER == cache->slab_order));

	cache_destroy(cache);
#endif
}

TEST_CASE("Cache's growing.") {
#if 0
	void *obj;
	cache_t *cache;

	cache = cache_create("test_cache", MAX_SIZE, 0);
	/* Growing off. */
	cache_growing_off(cache);
	obj = cache_alloc(cache);
	test_assert_null(obj);

	/* Growing on. */
	cache_growing_on(cache);
	obj = cache_alloc(cache);
	test_assert_not_null(obj);

	cache_destroy(cache);
#endif
}
#if 0
static size_t list_length(struct dlist_head *head) {
	struct dlist_head *pos;
	size_t cnt = 0;

	dlist_foreach(pos, head) {
		cnt++;
	}

	return cnt;
}
#endif
