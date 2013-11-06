/**
 * @file
 *
 * @date Oct 21, 2013
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <string.h>
#include <util/bitmap.h>

EMBOX_TEST_SUITE("util/bitmap test");

#define TEST_ALINGED_SZ    64
#define TEST_UNALINGED_SZ  100

TEST_CASE("aligned size with red zone after an array") {
	BITMAP_DECL(bitmap, TEST_ALINGED_SZ + 1);

	memset(bitmap, 0xAA, sizeof(bitmap));
	bitmap_clear_all(bitmap, TEST_ALINGED_SZ);

	test_assert_equal(bitmap_find_first_bit(bitmap, TEST_ALINGED_SZ),
			TEST_ALINGED_SZ /* not found */ );
}

TEST_CASE("unaligned size") {
	BITMAP_DECL(bitmap, TEST_UNALINGED_SZ);

	bitmap_clear_all(bitmap, TEST_UNALINGED_SZ);
	test_assert_equal(bitmap_find_first_bit(bitmap, TEST_UNALINGED_SZ),
			TEST_UNALINGED_SZ /* not found */ );

	for (int i = TEST_UNALINGED_SZ-1; i >= 0; --i) {
		bitmap_set_bit(bitmap, i);

		test_assert_not_zero(bitmap_test_bit(bitmap, i));
		test_assert_equal(bitmap_find_first_bit(bitmap, TEST_UNALINGED_SZ), i);
	}

}
