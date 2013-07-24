/**
 * @file
 *
 * @date Oct 10, 2012
 * @author: Anton Bondarev
 */



#include <embox/test.h>
#include <util/indexator.h>

EMBOX_TEST_SUITE("util/indexator_text");

TEST_SETUP(setup_indexator_test);

INDEX_DEF(idx32,0,32);
INDEX_DEF(idx256,0,256);

INDEX_DEF(idx16,0,16);
INDEX_DEF(idx_static,0,16);

TEST_CASE("incremental allocating indexes from static indexator") {
	int i;
	int idx;

	for(i = 0; i < idx_static.capacity; i++) {
		idx = index_alloc(&idx_static, INDEX_ALLOC_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("incremental allocating indexes") {
	int i;
	int idx;

	for(i = 0; i < idx32.capacity; i++) {
		idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("incremental allocating of next indexes") {
	int i;
	int idx;

	for(i = 0; i < idx32.capacity; i++) {
		idx = index_alloc(&idx32, INDEX_ALLOC_NEXT);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("allocation next index after allocation of min index") {
	int idx;

	idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
	test_assert_equal(idx, 0);
	idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
	test_assert_equal(idx, 1);

	index_free(&idx32, 1);

	idx = index_alloc(&idx32, INDEX_ALLOC_NEXT);
	test_assert_equal(idx, 2);
}

TEST_CASE("incremental allocating indexes for indexator is less than the word ") {
	int i;
	int idx;

	for(i = 0; i < idx16.capacity; i++) {
		idx = index_alloc(&idx16, INDEX_ALLOC_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("allocating from 2 indexes") {
	int idx;

	idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
	test_assert_equal(idx, 0);
	idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
	test_assert_equal(idx, 1);
	idx = index_alloc(&idx256, INDEX_ALLOC_MIN);
	test_assert_equal(idx, 0);
}

static int setup_indexator_test(void) {
	indexator_init(&idx32, 0, idx32_array, 32 / 32);
	indexator_init(&idx256, 0, idx256_array, 256 / 32);
	indexator_init(&idx16, 0, idx16_array, 16 / 32);

	return 0;
}
