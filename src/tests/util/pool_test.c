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
	int some_stuff[5];
};

#define POOL_SZ 13

POOL_DEF(struct object, pool, POOL_SZ);

TEST_CASE("Allocated objects must not exceed the pool boundaries") {
	for (int i = 0; i < POOL_SZ; ++i) {
		struct object *obj = (struct object *) static_cache_alloc(&pool);
		test_assert_not_null(obj);
		test_assert(obj >= (struct object *) pool.cache_begin);
		test_assert(obj < (struct object *) (pool.cache_begin
				+ ARRAY_SIZE(__pool_pool)));
	}
	test_assert_null(static_cache_alloc(&pool));
}

