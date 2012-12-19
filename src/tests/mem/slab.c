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
#include <util/dlist.h>
#include <mem/page.h>

static size_t list_length(struct dlist_head *head);

/* Used to fill slab with one object */
#define MAX_SIZE (PAGE_SIZE() - 64)

EMBOX_TEST_SUITE("slab allocator test");

TEST_CASE("Allocation in one slab without cache growing.") {
	void* obj;
	cache_t *cache;

	/* Cache for one objec of size to fit in one page. */
	cache = cache_create("cache1", MAX_SIZE, 0);

	/* Test initial cache's property: no slabs */
	test_assert_not_null(cache);
	test_assert_equal(cache->num, 1);
	test_assert_equal(cache->slab_order, 0);
	test_assert(dlist_empty(&cache->slabs_free));
	test_assert(dlist_empty(&cache->slabs_partial));
	test_assert(dlist_empty(&cache->slabs_full));

	/* Allocate object. */
	obj = cache_alloc(cache);
	test_assert_not_null(obj);
	test_assert(dlist_empty(&cache->slabs_free));
	test_assert_equal(list_length(&cache->slabs_full), 1);

	/* Free object. */
	cache_free(cache, obj);
	test_assert_equal(list_length(&cache->slabs_free), 1);
	test_assert(dlist_empty(&cache->slabs_full));

	cache_destroy(cache);
}

#if 0
TEST_CASE("Create several caches with different size and name.") {
	cache_t *cachep;
	char cache_name[0x10];

	for (int i = 0; i < 14; i++) {
		sprintf(cache_name, "cache_%d", 1 << i);
		cachep = cache_create("cache_name", 1 << i, 0);
		test_assert_not_null(cachep);
		cache_destroy(cachep);
	}
}
#endif

TEST_CASE("Allocation in multiple slabs with cache growing.") {
	void *obj;
	cache_t *cache;

	cache = cache_create("cache1", MAX_SIZE / 2, 0);

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
}

TEST_CASE("Cache shrinking.") {
	void *obj[2];
	cache_t *cache;

	cache = cache_create("cache1", MAX_SIZE, 1);

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
}

TEST_CASE("Slab size.") {
	/* Slab allocater store objects in slabs with size 2^n. It is very
	 * effective when low level allocator uses buddy strategy. So, test if slab size is appropriate. */

	cache_t *cache;
	size_t num, slab_size;
	/* Object with "bad" size */
	size_t obj_size = MAX_SIZE - (MAX_SIZE / MAX_INT_FRAGM_ORDER);

	/* Create object with wastage greater than (1 / MAX_INT_FRAGM_ORDER) * 100% */
	cache = cache_create("cache1", obj_size , 1);
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
}

static size_t list_length(struct dlist_head *head) {
	struct dlist_head *pos, *nxt;
	size_t cnt = 0;

	dlist_foreach(pos, nxt, head) {
		cnt++;
	}

	return cnt;
}
