/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.07.2014
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strchr testing suite");

static const char *test_strchr_alpha = "abcdefghijklmnopqrstuvwxyz";

TEST_CASE("strchr should be able to find every character") {
	int i;

	for (i = 0; i < strlen(test_strchr_alpha); i++) {
		test_assert_equal(&test_strchr_alpha[i], strchr(test_strchr_alpha, test_strchr_alpha[i]));
	}
}

TEST_CASE("strchr shouldn't find a char if it's not in string") {
	test_assert_null(strchr("abc", 'd'));
	test_assert_null(strchr("abc", 'e'));
	test_assert_null(strchr("abc", 'f'));
}

TEST_CASE("strchr should find null terminator of a string") {
	const char *str1 = "abc";
	const char *str2 = "abcd";
	const char *str3 = "abcdf";

	test_assert_equal(str1 + 3, strchr(str1, '\0'));
	test_assert_equal(str2 + 4, strchr(str2, '\0'));
	test_assert_equal(str3 + 5, strchr(str3, '\0'));
}

TEST_CASE("strchr should find first occurency of character") {
	const char *str = "aaaabbbb";

	test_assert_equal(str, strchr(str, 'a'));
	test_assert_equal(str, strchr(str, 'a'));

	test_assert_equal(str + 4, strchr(str, 'b'));
	test_assert_equal(str + 4, strchr(str, 'b'));
}
