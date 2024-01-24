/**
 * @file
 *
 * @date Sep 20, 2012
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <lib/libds/bit.h>
#include <lib/libds/array.h>

EMBOX_TEST_SUITE("util/bit test");

TEST_CASE("some obvious checks for bit_ffs") {
	test_assert_equal(bit_ffs(0x00000000ul), 0);
	test_assert_equal(bit_ffs(0x00000001ul), 1);
	test_assert_equal(bit_ffs(0x00000002ul), 2);
	test_assert_equal(bit_ffs(0x0000000ful), 1);
	test_assert_equal(bit_ffs(0x000f000ful), 1);
	test_assert_equal(bit_ffs(0x000f0000ul), 17);
	test_assert_equal(bit_ffs(0xfffffffful), 1);
	test_assert_equal(bit_ffs(0x80000000ul), 32);
}

TEST_CASE("some obvious checks for bit_fls") {
	test_assert_equal(bit_fls(0x00000000ul), 0);
	test_assert_equal(bit_fls(0x00000001ul), 1);
	test_assert_equal(bit_fls(0x00000002ul), 2);
	test_assert_equal(bit_fls(0x0000000ful), 4);
	test_assert_equal(bit_fls(0x000f000ful), 20);
	test_assert_equal(bit_fls(0x000f0000ul), 20);
	test_assert_equal(bit_fls(0xfffffffful), 32);
	test_assert_equal(bit_fls(0x80000000ul), 32);
}

TEST_CASE("generic test for bit_ffs and bit_fls") {
	for (int i = 0; i < sizeof(unsigned long); ++i) {
		test_assert_equal(bit_ffs(0x1ul << i), i + 1);
		test_assert_equal(bit_fls(0x1ul << i), i + 1);
	}
}

TEST_CASE("generic test for bit_ctz and bit_clz") {
	for (int i = 0; i < sizeof(unsigned long); ++i) {
		test_assert_equal(bit_ctz(0x1ul << i), i);
		test_assert_equal(bit_clz(0x1ul << i),
				sizeof(unsigned long) * 8 - i - 1);
	}
}

TEST_CASE("test for bit_foreach") {
	const unsigned long patterns[] = { 0, 1, 2, 3, 5, 7, 11, 42, 0xf00,
			0xc0ffee, 0x0ff1ce, 0xdeadbeef, 0x12345678,
			0xadadadad, 0xa5a5a5a5, 0xaaaaaaaa, 0x55555555,
			0xffffffff };
	unsigned long pat;

	array_foreach(pat, patterns, ARRAY_SIZE(patterns)) {
		unsigned long check = 0;
		int bit;

		bit_foreach(bit, pat) {
			check |= 1ul << bit;
		}

		test_assert_equal(check, pat);
	}
}
