/**
 * @file
 *
 * @date Oct 21, 2013
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <util/bitmap.h>

EMBOX_TEST_SUITE("util/bitmap test");

#define TEST_BITMAP_SZ 100

TEST_CASE() {
	BITMAP_DECL(bitmap, TEST_BITMAP_SZ);

	bitmap_clear_all(bitmap, TEST_BITMAP_SZ);
	test_assert_equal(bitmap_find_first_set_bit(bitmap, TEST_BITMAP_SZ),
			TEST_BITMAP_SZ /* not found */ );

	for (int i = TEST_BITMAP_SZ-1; i >= 0; --i) {
		bitmap_set_bit(bitmap, i);

		test_assert_not_zero(bitmap_test_bit(bitmap, i));
		test_assert_equal(bitmap_find_first_set_bit(bitmap, TEST_BITMAP_SZ), i);
	}

}
