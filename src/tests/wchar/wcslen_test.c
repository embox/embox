/**
 * @file wcslen_test.c
 *
 * @date July 1, 2022
 * @author Dmitry Lopatinsky
 */

#include <embox/test.h>
#include <wchar.h>
#include <wctype.h>

EMBOX_TEST_SUITE("wcslen test suite");

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

