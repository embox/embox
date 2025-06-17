/**
 * @file
 *
 * @date Feb 20, 2021
 * @author: Vedant Paranjape
 */

#include <wchar.h>
#include <embox/test.h>

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
