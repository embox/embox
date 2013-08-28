/**
 * @file
 *
 * @date Aug 26, 2013
 * @author: Anton Bondarev
 */

#include <strings.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("string libriry (strcasecmp)");

TEST_CASE("strcasecmp compare identical strings") {
	char str1[] = "aa";
	char str2[] = "aa";

	test_assert_zero(strcasecmp(str1, str2));
}

TEST_CASE("strcasecmp compare equal strings with different cases") {
	char str1[] = "aaBB";
	char str2[] = "AAbb";

	test_assert_zero(strcasecmp(str1, str2));
}

TEST_CASE("strcasecmp compare string ") {
	char str1[] = "aa";
	char str2[] = "aa";

	test_assert_zero(strcasecmp(str1, str2));
}

TEST_CASE("strcasecmp compare strings ") {
	char str1[] = "aaBB\n";
	char str2[] = "AAbb";

	test_assert_equal(strcasecmp(str1, str2), '\n');
}

TEST_CASE("strncasecmp compare strings ") {
	char str1[] = "aaBB\n";
	char str2[] = "AAbb";

	test_assert_zero(strncasecmp(str1, str2, 4));
	test_assert_equal(strncasecmp(str1, str2, 5), '\n');
}
