/**
 * @file
 * @brief Test unit for stdlib/mbstowcs and stdlib/mbtowc.
 *
 * @date Nov 16, 2018
 * @author Chubukov Filipp
 */

#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_STRING "ABC"
#define CHECK_STRING L"ABC"

EMBOX_TEST_SUITE("stdlib/mbstowcs_test and stdlib/mbtowc");

TEST_CASE("mbstowcs compare ABC") {
	int length = 0;
	char  test_string[] = TEST_STRING;
	wchar_t check_string[] = CHECK_STRING;
	wchar_t buffer[10];

	length = mbstowcs(buffer, test_string, sizeof(test_string));
	test_assert(length == sizeof(test_string) - 1);
	test_assert(memcmp(buffer, check_string, length) == 0);
}

TEST_CASE("mbtowc compare ABC") {
	int length = 0;
	char test_string[] = TEST_STRING;
	wchar_t check_string[] = L"A"; /* Just first symbol of test_string */
	wchar_t buffer[10];

	length = mbtowc(buffer, test_string, sizeof(char));
	test_assert(length == sizeof(char));
	test_assert(buffer[0] == check_string[0]);
} 

TEST_CASE("mbstowcs compare terminating null character") {
	int length = 0;
	char test_string[] = "\0";
	wchar_t check_string[] = L"\0";
	wchar_t buffer[10];

	length = mbstowcs(buffer, test_string, sizeof(test_string));
	test_assert(length == 0);
	test_assert(buffer[0] == check_string[0]);
}
