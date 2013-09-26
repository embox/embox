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

	for(i = 0; i < index_capacity(&idx_static); i++) {
		idx = index_alloc(&idx_static, INDEX_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("incremental allocating indexes") {
	int i;
	int idx;

	for(i = 0; i < index_capacity(&idx32); i++) {
		idx = index_alloc(&idx32, INDEX_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("incremental allocating of next indexes") {
	int i;
	int idx;

	for(i = 0; i < index_capacity(&idx32); i++) {
		idx = index_alloc(&idx32, INDEX_NEXT);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("allocation next index after allocation of min index") {
	int idx;

	idx = index_alloc(&idx32, INDEX_MIN);
	test_assert_equal(idx, 0);
	idx = index_alloc(&idx32, INDEX_MIN);
	test_assert_equal(idx, 1);

	index_free(&idx32, 1);

	idx = index_alloc(&idx32, INDEX_NEXT);
	test_assert_equal(idx, 2);
}

TEST_CASE("incremental allocating indexes for indexator is less than the word ") {
	int i;
	int idx;

	for(i = 0; i < index_capacity(&idx16); i++) {
		idx = index_alloc(&idx16, INDEX_MIN);
		test_assert_equal(i, idx);
	}
}

TEST_CASE("allocating from 2 indexes") {
	int idx;

	idx = index_alloc(&idx32, INDEX_MIN);
	test_assert_equal(idx, 0);
	idx = index_alloc(&idx32, INDEX_MIN);
	test_assert_equal(idx, 1);
	idx = index_alloc(&idx256, INDEX_MIN);
	test_assert_equal(idx, 0);
}

static int setup_indexator_test(void) {
	index_init(&idx32, 0, 32 / 32, idx32_data);
	index_init(&idx256, 0, 256 / 32, idx256_data);
	index_init(&idx16, 0, 16 / 32, idx16_data);

	return 0;
}
