/**
 * @file
 * @brief Test unit for stdlib/wcstombs and stdlib/wctomb.
 *
 * @date Nov 16, 2018
 * @author Chubukov Filipp
 */

#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_STRING L"ABC"
#define CHECK_STRING "ABC"

EMBOX_TEST_SUITE("stdlib/wcstombs_test and stdlib/wctomb");

TEST_CASE("wcstombs compare ABC") {
	int length = 0;
	char check_string[] = CHECK_STRING;
	wchar_t test_string[] = TEST_STRING;
	char buffer[10];

	length = wcstombs(buffer, test_string, sizeof(check_string));
	test_assert(length == sizeof(check_string) - 1);
	test_assert(memcmp(buffer, check_string, length) == 0);
}

TEST_CASE("wctomb compare ABC") {
	int length = 0;
	char check_string[] = "A";/* Just first symbol of test_string */
	wchar_t test_string[] = TEST_STRING;
	char buffer[10];

	length = wctomb(buffer, test_string);
	test_assert(length == sizeof(char));
	test_assert(buffer[0] == check_string[0]);
}

TEST_CASE("wctombs compare terminating null character") {
	int length = 0;
	char check_string[] = "\0";
	wchar_t test_string[] = L"\0";
	char buffer[10];

	length = wcstombs(buffer, test_string, 1);
	test_assert(length == 0);
	test_assert(buffer[0] == check_string[0]);
}
