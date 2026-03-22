/**
 * @file
 * @brief Test unit for stdlib/itoa.
 *
 * @date Mar 22, 2026
 * @author Matt Van Horn
 */

#include <embox/test.h>
#include <stdlib.h>
#include <string.h>

EMBOX_TEST_SUITE("stdlib/itoa_test");

TEST_CASE("itoa positive number base 10") {
	char buf[32];

	itoa(12345, buf, 10);
	test_assert_str_equal(buf, "12345");
}

TEST_CASE("itoa negative number base 10") {
	char buf[32];

	itoa(-42, buf, 10);
	test_assert_str_equal(buf, "-42");
}

TEST_CASE("itoa zero") {
	char buf[32];

	itoa(0, buf, 10);
	test_assert_str_equal(buf, "0");
}

TEST_CASE("itoa base 16") {
	char buf[32];

	itoa(255, buf, 16);
	test_assert_str_equal(buf, "ff");
}

TEST_CASE("itoa base 2") {
	char buf[32];

	itoa(10, buf, 2);
	test_assert_str_equal(buf, "1010");
}

TEST_CASE("itoa invalid base returns empty string") {
	char buf[32];

	itoa(123, buf, 1);
	test_assert_str_equal(buf, "");

	itoa(123, buf, 37);
	test_assert_str_equal(buf, "");
}
