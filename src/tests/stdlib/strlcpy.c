/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strlcpy test suit");

TEST_CASE("strlcpy with zero size") {
	char src[] = "string";
	char dst[sizeof(src) + 1];


	test_assert_equal(strlen(src), strlcpy(dst, src, 0));
}

TEST_CASE("strlcpy part of a string") {
	char src[] = "string";
	char dst[sizeof(src) + 1];

	memset(dst, 0x55, sizeof(dst));

	test_assert_equal(sizeof(src) - 1, strlcpy(dst, src, sizeof(src)));
	test_assert_zero(memcmp(dst, src, sizeof(src) - 1));
}

TEST_CASE("copy full string with strlcpy") {
	char src[] = "string";
	char dst[sizeof(src) + 1];

	memset(dst, 0x55, sizeof(dst));

	test_assert_equal(strlen(src), strlcpy(dst, src, sizeof(dst)));
	test_assert_zero(strcmp(dst, src));
}
