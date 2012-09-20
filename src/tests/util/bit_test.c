/**
 * @file
 * @brief TODO
 *
 * @date Sep 20, 2012
 * @author eldar
 */

#include <embox/test.h>

#include <util/bit.h>

EMBOX_TEST_SUITE("util/bit test");

TEST_CASE("some obvious checks for bit_ffs") {
	test_assert_equal(bit_ffs(0x00000000ul), 0);
	test_assert_equal(bit_ffs(0x00000001ul), 1);
	test_assert_equal(bit_ffs(0x00000002ul), 2);
	test_assert_equal(bit_ffs(0x0000000ful), 1);
	test_assert_equal(bit_ffs(0x000f000ful), 1);
	test_assert_equal(bit_ffs(0x000f0000ul), 17);
	test_assert_equal(bit_ffs(0xfffffffful),  1);
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

