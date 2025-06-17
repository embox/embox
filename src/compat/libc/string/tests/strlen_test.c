/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for strlen()");

TEST_CASE("get length with strnlen()") {
	char str[] = "string";
	size_t len;

	len = sizeof(str) - 1;

	test_assert_equal(len, strlen(str));
}

TEST_CASE("use strnlen() with maxlen less than string length") {
	char str[] = "string";
	size_t len;

	len = sizeof(str) - 1;

	test_assert_equal(len, strnlen(str, len));
}
