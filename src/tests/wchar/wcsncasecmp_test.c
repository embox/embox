/**
 * @file
 *
 * @date June 6, 2022
 * @author Aleksey Zhmulin
 * @author Daniil Nikolaev
 */

#include <embox/test.h>
#include <wchar.h>
#include <wctype.h>

EMBOX_TEST_SUITE("test suite for wcsncasecmp()");

TEST_CASE("wcsncasecmp compare a string to itself") {
    wchar_t ws1[] = L"Hello";
    
    test_assert_zero(wcsncasecmp(ws1, ws1, 5));
}

TEST_CASE("wcsncasecmp compare strings with non-alphabetic characters") {
    wchar_t ws1[] = L"123 ABC";
    wchar_t ws2[] = L"123 abc";

    test_assert_zero(wcsncasecmp(ws1, ws2, 7));
}

TEST_CASE("wcsncasecmp compare strings with n equal to zero") {
    wchar_t ws1[] = L"Hello";
    wchar_t ws2[] = L"World";

    test_assert_zero(wcsncasecmp(ws1, ws2, 0));
}

TEST_CASE("wcsncasecmp compare empty wide-character strings") {
	wchar_t ws1[] = L"";
	wchar_t ws2[] = L"";

	test_assert_zero(wcsncasecmp(ws1, ws2, 1));
}

TEST_CASE("wcsncasecmp compare identical wide-character strings") {
	wchar_t ws1[] = L"abc";
	wchar_t ws2[] = L"abc";

	test_assert_zero(wcsncasecmp(ws1, ws2, 3));
}

TEST_CASE("wcsncasecmp compare equal wide-character strings with different"
		" cases") {
	wchar_t ws1[] = L"aBc";
	wchar_t ws2[] = L"AbC";

	test_assert_zero(wcsncasecmp(ws1, ws2, 3));
}

TEST_CASE("wcsncasecmp compare different wide-character strings (n == 0)") {
	wchar_t ws1[] = L"aa";
	wchar_t ws2[] = L"bb";

	test_assert_zero(wcsncasecmp(ws1, ws2, 0));
}

TEST_CASE("wcsncasecmp compare different wide-character strings"
		" (n == length of equal part of the strings)") {
	wchar_t ws1[] = L"AbcD";
	wchar_t ws2[] = L"aBcE";

	test_assert_zero(wcsncasecmp(ws1, ws2, 3));
}

TEST_CASE("wcsncasecmp compare equal wide-character strings"
		" (n > max string length") {
	wchar_t ws1[] = L"aBc";
	wchar_t ws2[] = L"abc";

	test_assert_zero(wcsncasecmp(ws1, ws2, 20));
}

TEST_CASE("wcsncasecmp compare different wide-character strings (ws1 > ws2)") {
	wchar_t ws1[] = L"aaBB\n";
	wchar_t ws2[] = L"AAbb";

	test_assert_equal(wcsncasecmp(ws1, ws2, 5), '\n');
}

TEST_CASE("wcsncasecmp compare different wide-character strings (ws1 < ws2") {
	wchar_t ws1[] = L"aaBB";
	wchar_t ws2[] = L"AAbb\n";

	test_assert_equal(wcsncasecmp(ws1, ws2, 5), -'\n');
}

TEST_CASE("wcsncasecmp compare wide-character strings of different lengths"
	" (ws1 < ws2)") {
    wchar_t ws1[] = L"Hello";
    wchar_t ws2[] = L"Hello World";

    test_assert_equal(wcsncasecmp(ws1, ws2, 6), -' ');
}

TEST_CASE("wcsncasecmp compare wide-character strings of different lengths"
		" (ws1 > ws2)") {
    wchar_t ws1[] = L"Hello World";
    wchar_t ws2[] = L"Hello";

    test_assert_equal(wcsncasecmp(ws1, ws2, 6), ' ');
}