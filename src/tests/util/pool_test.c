/**
 * @file
 * @brief Tests object pool utility.
 *
 * @date Mar 29, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <util/array.h>

#include <util/pool.h>

EMBOX_TEST_SUITE("util/pool test");

struct object {
	char some_stuff[5];
};

#define MY_POOL_SZ 13

POOL_DEF(struct object, my_pool, MY_POOL_SZ);
static struct object *objects[MY_POOL_SZ];

#define pool_alloc static_cache_alloc
#define pool_free  static_cache_free

static void do_alloc(struct pool *, struct object *objects[], int nr);
static void do_free(struct pool *, struct object *objects[], int nr);

TEST_CASE("Allocated objects must not exceed the pool boundaries") {
	struct object *obj;
	for (int i = 0; i < MY_POOL_SZ; ++i) {
		obj = objects[i] = (struct object *) static_cache_alloc(&my_pool);
		test_assert_not_null(obj);
		test_assert(obj >= (struct object *) my_pool.cache_begin);
		test_assert(obj < (struct object *) (my_pool.cache_begin
						+ ARRAY_SIZE(__my_pool_pool)));
	}
	test_assert_null(static_cache_alloc(&my_pool));
	do_free(&my_pool, objects, MY_POOL_SZ);
}

TEST_CASE("pool_alloc should return NULL when the pool becomes full") {
	do_alloc(&my_pool, objects, MY_POOL_SZ);
	test_assert_null(pool_alloc(&my_pool));
	do_free(&my_pool, objects, MY_POOL_SZ);
}

TEST_CASE("After freeing all objects using pool_free one should be able to "
		"allocate the same number of objects again") {
	for (int i = 0; i < 2; ++i) {
		do_alloc(&my_pool, objects, MY_POOL_SZ);
		test_assert_null(pool_alloc(&my_pool));
		do_free(&my_pool, objects, MY_POOL_SZ);
	}
}

static void do_alloc(struct pool *pool, struct object *objects[], int nr) {
	for (int i = 0; i < nr; ++i) {
		test_assert_not_null(
				(objects[i] = (struct object *) static_cache_alloc(pool)));
	}
}

static void do_free(struct pool *pool, struct object *objects[], int nr) {
	struct object *object;
	for (int i = 0; i < nr; ++i) {
		if ((object = objects[i])) {
			pool_free(pool, object);
		}
	}
}

