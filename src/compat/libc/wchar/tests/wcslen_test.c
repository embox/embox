/**
 * @file
 *
 * @date Feb 20, 2021
 * @author Vedant Paranjape
 * @author Dmitry Lopatinsky
 */

#include <embox/test.h>
#include <wchar.h>
#include <wctype.h>

EMBOX_TEST_SUITE("test suit for wcslen()");

TEST_CASE("get length with wcslen()") {
	wchar_t str[] = L"covid19sucks";
	size_t len;

	len = sizeof(str) / 4 - 1;

	test_assert_equal(len, wcslen(str));
}

TEST_CASE("use wcsnlen() with maxlen less than string length") {
	wchar_t str[] = L"covid19sucks";
	size_t len;

	len = sizeof(str) / 4 - 1;

	test_assert_equal(len, wcsnlen(str, len));
}

TEST_CASE("normal string") {
	wchar_t str[] = L"abcde";

	test_assert_equal(wcslen(str), 5);
}

TEST_CASE("string with a \\0 in the middle") {
	wchar_t str[] = L"abc\0de";

	test_assert_equal(wcslen(str), 3);
}

TEST_CASE("string with a \\0 as the first character") {
	wchar_t str[] = L"\0abcde";

	test_assert_zero(wcslen(str));
}

