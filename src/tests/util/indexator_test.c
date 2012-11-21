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

TEST_CASE("incremental allocating indexes") {
	int i;
	int idx;

	for(i = 0; i < idx32.capacity; i++) {
		idx = index_alloc(&idx32, INDEX_ALLOC_MIN);
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
	indexator_init(&idx32, 0, idx32_array, 1);
	indexator_init(&idx256, 0, idx256_array, 256/32);

	return 0;
}
