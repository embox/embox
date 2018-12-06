/**
 * @file
 * @brief Test unit for stdlib/mbtowc.
 *
 * @date Nov 16, 2018
 * @author Chubukov Filipp
 */

#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EMBOX_TEST_SUITE("stdlib/wcstombs_test");

TEST_CASE("compare ABC") {
	int length = 0;
	char  check[] = "ABC";
	wchar_t string[] = L"ABC";
	char buffer[10];

	length = wcstombs(buffer, string, 3);
	test_assert(length != -1);
	test_assert(memcmp(buffer,check,length) == 0);
}


