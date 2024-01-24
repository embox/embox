/**
 * @file
 *
 * @date Oct 21, 2013
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <string.h>
#include <lib/libds/bitmap.h>

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
#define TEST_3WORD (3 * LONG_BIT)
TEST_CASE("find zero on 3 words") {
	BITMAP_DECL(bitmap, TEST_3WORD);
	int start, len, zero;

	bitmap_set_all(bitmap, TEST_3WORD);

	for (len = 0; len < TEST_3WORD; len++) {
		for (start = 0; start < TEST_3WORD; start++) {
			for (zero = 0; zero < TEST_3WORD; zero++) {
				int ans;

				bitmap_clear_bit(bitmap, zero);
				ans = start <= zero && zero < len ? zero : len;
				test_assert_equal(ans, bitmap_find_zero_bit(bitmap, len, start));
				bitmap_set_bit(bitmap, zero);
			}
		}
	}
}
