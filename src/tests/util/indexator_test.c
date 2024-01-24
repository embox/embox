/**
 * @file
 * @brief
 *
 * @date 10.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <errno.h>
#include <stddef.h>
#include <lib/libds/indexator.h>

EMBOX_TEST_SUITE("util/indexator_text");

TEST_SETUP(setup_indexator_test);

#define IDX_START 0x02
#define IDX_SIZE  0x10

#define IDX_CLAMP_MIN 5
#define IDX_CLAMP_MAX 7

INDEX_DEF(idx, IDX_START, IDX_SIZE);

TEST_CASE("allocating minimal index") {
	size_t i;

	for (i = index_start(&idx); i <= index_end(&idx); ++i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_MIN));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));

	index_unlock(&idx, 3);
	index_unlock(&idx, 4);

	test_assert_equal(3, index_alloc(&idx, INDEX_MIN));

	index_unlock(&idx, 2);

	test_assert_equal(2, index_alloc(&idx, INDEX_MIN));
	test_assert_equal(4, index_alloc(&idx, INDEX_MIN));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));
}

TEST_CASE("allocating maximum index") {
	size_t i;

	for (i = index_end(&idx); i >= index_start(&idx); --i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_MAX));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MAX));

	index_unlock(&idx, 3);
	index_unlock(&idx, 4);

	test_assert_equal(4, index_alloc(&idx, INDEX_MAX));

	index_unlock(&idx, 5);

	test_assert_equal(5, index_alloc(&idx, INDEX_MAX));
	test_assert_equal(3, index_alloc(&idx, INDEX_MAX));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MAX));
}

TEST_CASE("allocating previous index") {
	size_t i;

	for (i = index_end(&idx); i >= index_start(&idx); --i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_PREV));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_PREV));

	index_unlock(&idx, 3);
	index_unlock(&idx, 4);

	test_assert_equal(4, index_alloc(&idx, INDEX_PREV));

	index_unlock(&idx, 4);
	index_unlock(&idx, 5);

	test_assert_equal(3, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(5, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(4, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_PREV));
}

TEST_CASE("allocating next index") {
	size_t i;

	for (i = index_start(&idx); i <= index_end(&idx); ++i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_NEXT));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_NEXT));

	index_unlock(&idx, 3);
	index_unlock(&idx, 4);

	test_assert_equal(3, index_alloc(&idx, INDEX_NEXT));

	index_unlock(&idx, 3);
	index_unlock(&idx, 2);

	test_assert_equal(4, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(2, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(3, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_NEXT));
}

TEST_CASE("allocating random index") {
	size_t i;

	for (i = 0; i < index_capacity(&idx); ++i) {
		test_assert_not_equal(INDEX_NONE, index_alloc(&idx,
					INDEX_RANDOM));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_RANDOM));

	index_unlock(&idx, 3);

	test_assert_equal(3, index_alloc(&idx, INDEX_RANDOM));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_RANDOM));
}

TEST_CASE("allocating minimal index from interval") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = index_clamp_min(&idx);
			i <= index_clamp_max(&idx); ++i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_MIN));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));

	index_unlock(&idx, 6);
	index_unlock(&idx, 7);

	test_assert_equal(6, index_alloc(&idx, INDEX_MIN));

	index_unlock(&idx, 5);

	test_assert_equal(5, index_alloc(&idx, INDEX_MIN));
	test_assert_equal(7, index_alloc(&idx, INDEX_MIN));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));

}

TEST_CASE("allocating maximum index from interval") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = index_clamp_max(&idx);
			i >= index_clamp_min(&idx); --i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_MAX));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MAX));

	index_unlock(&idx, 5);
	index_unlock(&idx, 6);

	test_assert_equal(6, index_alloc(&idx, INDEX_MAX));

	index_unlock(&idx, 7);

	test_assert_equal(7, index_alloc(&idx, INDEX_MAX));
	test_assert_equal(5, index_alloc(&idx, INDEX_MAX));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MAX));
}

TEST_CASE("allocating previous index from interval") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = index_clamp_max(&idx);
			i >= index_clamp_min(&idx); --i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_PREV));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_PREV));

	index_unlock(&idx, 5);
	index_unlock(&idx, 6);

	test_assert_equal(6, index_alloc(&idx, INDEX_PREV));

	index_unlock(&idx, 6);
	index_unlock(&idx, 7);

	test_assert_equal(5, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(7, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(6, index_alloc(&idx, INDEX_PREV));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_PREV));
}

TEST_CASE("allocating next index from interval") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = index_clamp_min(&idx);
			i <= index_clamp_max(&idx); ++i) {
		test_assert_equal(i, index_alloc(&idx, INDEX_NEXT));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_NEXT));

	index_unlock(&idx, 6);
	index_unlock(&idx, 7);

	test_assert_equal(6, index_alloc(&idx, INDEX_NEXT));

	index_unlock(&idx, 6);
	index_unlock(&idx, 5);

	test_assert_equal(7, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(5, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(6, index_alloc(&idx, INDEX_NEXT));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_NEXT));
}

TEST_CASE("allocating random index from interval") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = 0; i < IDX_CLAMP_MAX - IDX_CLAMP_MIN + 1; ++i) {
		test_assert_not_equal(INDEX_NONE, index_alloc(&idx,
					INDEX_RANDOM));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_RANDOM));

	index_unlock(&idx, 7);

	test_assert_equal(7, index_alloc(&idx, INDEX_RANDOM));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_RANDOM));
}

TEST_CASE("allocating index from interval after locking") {
	size_t i;

	index_clamp(&idx, IDX_CLAMP_MIN, IDX_CLAMP_MAX);

	for (i = index_start(&idx);
			i != index_start(&idx) + index_capacity(&idx);
			++i) {
		test_assert_true(index_try_lock(&idx, i));
	}
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));

	test_assert_false(index_try_lock(&idx, 2));
	test_assert_false(index_try_lock(&idx, 5));

	index_unlock(&idx, 2);
	index_unlock(&idx, 5);

	test_assert_equal(5, index_alloc(&idx, INDEX_MIN));
	test_assert_equal(INDEX_NONE, index_alloc(&idx, INDEX_MIN));

	test_assert_true(index_try_lock(&idx, 2));
	test_assert_false(index_try_lock(&idx, 5));
}

static int setup_indexator_test(void) {
	index_clean(&idx);
	return 0;
}
