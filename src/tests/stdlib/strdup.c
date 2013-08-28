/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for strndup()");

TEST_CASE("duplicate full string with strndup()") {
	char src[] = "string";
	char *dst;

	dst = strndup(src, sizeof(src));
	test_assert_not_null(dst);
	test_assert_zero(strcmp(dst, src));
}

TEST_CASE("duplicate part of a string with strndup()") {
	char src[] = "string";
	char *dst;

	dst = strndup(src, sizeof(src) - 1);
	test_assert_not_null(dst);
	test_assert_zero(strncmp(dst, src, sizeof(src) - 1));
}
