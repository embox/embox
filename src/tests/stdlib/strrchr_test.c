/**
 * @file
 * @brief Testing strrchr function
 *
 * @author  Tiriplica Vlad Petre
 * @date    27.03.2020
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strrchr testing suite");

TEST_CASE("strrchr should be able to find every character") {
	const char *str = "abcdefghijklmnopqrstuvwxyz";
	int i;
	int len = strlen(str);

	for (i = 0; i < len; ++i) {
		test_assert_equal(&str[i], strrchr(str, str[i]));
	}
}

TEST_CASE("strrchr shouldn't find a char if it's not in string") {
	test_assert_null(strrchr("abc", 'd'));
	test_assert_null(strrchr("abc", 'e'));
	test_assert_null(strrchr("abc", 'f'));
}

TEST_CASE("strrchr should find null terminator of a string") {
	const char *str1 = "abc";
	const char *str2 = "abcd";
	const char *str3 = "abcdf";

	test_assert_equal(str1 + 3, strrchr(str1, '\0'));
	test_assert_equal(str2 + 4, strrchr(str2, '\0'));
	test_assert_equal(str3 + 5, strrchr(str3, '\0'));
}

TEST_CASE("strrchr should find last occurency of character") {
	const char *str1 = "aaaabbbb";
	const char *str2 = "abababab";
	const char *str3 = "abcddcba";

	test_assert_equal(str1 + 3, strrchr(str1, 'a'));
	test_assert_equal(str2 + 6, strrchr(str2, 'a'));
	test_assert_equal(str3 + 7, strrchr(str3, 'a'));
}
