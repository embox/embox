/**
 * @file
 *
 * @date Feb 19, 2021
 * @author: Zhou Ziqi
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for memset()");

TEST_CASE("memset string with empty string") {
	char str[] = "string";
	char exp[] = "";

	memset(str, '', sizeof(str));

	test_assert_str_equal(str, exp);
}

TEST_CASE("memset first n characters") {
	char str[] = "string";
	char exp[] = "---ing";

	memset(str, '-', 3);

	test_assert_str_equal(str, exp);
}

TEST_CASE("memset mystring with -1") {
	char str[] = "mystring";
	unsigned char exp[] = {-1, -1};

	memset(str, -1, sizeof(str));

	test_assert_str_equal(str, exp);
}

TEST_CASE("initialize an integer as zero") {
	char a[4];

	memset(a, 0, sizeof(a));

	test_assert_zero(a);
}

TEST_CASE("initialize an integer as infinite") {
	char a[4];

	memset(a, 0x3f, sizeof(a));

	test_assert_zero((int)a - 0x3f3f3f3f);
}
