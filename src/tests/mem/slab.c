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
#include <mem/page.h>

static size_t list_length(struct list_head *head);

/* Used to fill slab with one object */
#define MAX_SIZE (PAGE_SIZE() - 64)

/* Use to fill onepaged slab */

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
	test_assert(list_empty(&cache->slabs_free));
	test_assert(list_empty(&cache->slabs_partial));
	test_assert(list_empty(&cache->slabs_full));

	/* Allocate object. */
	obj = cache_alloc(cache);
	test_assert_not_null(obj);
	test_assert(list_empty(&cache->slabs_free));
	test_assert_equal(list_length(&cache->slabs_full), 1);

	/* Free object. */
	cache_free(cache, obj);
	test_assert_equal(list_length(&cache->slabs_free), 1);
	test_assert(list_empty(&cache->slabs_full));

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
	test_assert_equal(cache->num, 2);
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
	test_assert(list_empty(&cache->slabs_free));
}

TEST_CASE("Slab size.") {
	/* Slab allocater store objects in slabs with size 2^n. It is very
	 * effective when low level allocator uses buddy strategy. So, test if slab size is appropriate. */

	cache_t *cache;

	/* For object of size 1.3 pages is fit 4 pages, no 1 or 2 (because 1.3 * 3 = 3.9) */
	cache = cache_create("cache1", MAX_SIZE + (MAX_SIZE / 3), 1);
	test_assert_equal(cache->slab_order, 2);
	cache_destroy(cache);
}

static size_t list_length(struct list_head *head) {
	struct list_head *pos;
	size_t cnt = 0;

	list_for_each(pos, head) {
		cnt++;
	}

	return cnt;
}
