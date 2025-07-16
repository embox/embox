/**
 * @file
 * @brief Testing strchrnul function
 *
 * @author  Tiriplica Vlad Petre
 * @date    27.03.2020
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strchrnulnul testing suite");

TEST_CASE("strchrnul should be able to find every character") {
	const char *str = "abcdefghijklmnopqrstuvwxyz";
	int length = strlen(str);
	int i;

	for (i = 0; i < length; ++i) {
		test_assert_equal(&str[i], strchrnul(str, str[i]));
	}
}

TEST_CASE("strchrnul should return end of string char") {
	const char *str = "abc";

	test_assert_equal(str + 3, strchrnul("abc", 'd'));
	test_assert_equal(str + 3, strchrnul("abc", 'e'));
	test_assert_equal(str + 3, strchrnul("abc", 'f'));
}

TEST_CASE("strchrnul should find null terminator of a string") {
	const char *str1 = "abc";
	const char *str2 = "abcd";
	const char *str3 = "abcdf";

	test_assert_equal(str1 + 3, strchrnul(str1, '\0'));
	test_assert_equal(str2 + 4, strchrnul(str2, '\0'));
	test_assert_equal(str3 + 5, strchrnul(str3, '\0'));
}

TEST_CASE("strchrnul should find first occurency of character") {
	const char *str1 = "aaaabbbb";
	const char *str2 = "abababab";
	const char *str3 = "abcddcba";

	test_assert_equal(str1, strchrnul(str1, 'a'));
	test_assert_equal(str2 + 1, strchrnul(str2, 'b'));
	test_assert_equal(str3 + 3, strchrnul(str3, 'd'));
}
