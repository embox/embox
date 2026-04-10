/**
 * @file
 *
 * @date Apr 02, 2026
 * @author: Trevin Chow
 */

#include <string.h>
#include <strings.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string library (bzero)");

TEST_CASE("bzero zeros out a buffer") {
	char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};

	bzero(buf, sizeof(buf));

	for (int i = 0; i < 8; i++) {
		test_assert_zero(buf[i]);
	}
}

TEST_CASE("bzero with zero length does not modify buffer") {
	char buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};

	bzero(buf, 0);

	test_assert_equal((unsigned char)buf[0], 0xAA);
	test_assert_equal((unsigned char)buf[1], 0xBB);
	test_assert_equal((unsigned char)buf[2], 0xCC);
	test_assert_equal((unsigned char)buf[3], 0xDD);
}

TEST_CASE("bzero partial buffer") {
	char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};

	bzero(buf, 4);

	for (int i = 0; i < 4; i++) {
		test_assert_zero(buf[i]);
	}
	test_assert_equal(buf[4], 5);
	test_assert_equal(buf[5], 6);
	test_assert_equal(buf[6], 7);
	test_assert_equal(buf[7], 8);
}

TEST_CASE("bzero single byte") {
	char buf[1] = {0xFF};

	bzero(buf, 1);

	test_assert_zero(buf[0]);
}
