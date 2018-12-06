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

EMBOX_TEST_SUITE("stdlib/mbtowc_test");

TEST_CASE("compare ABC") {
	int length = 0;
	char  string[] = "ABC";
	wchar_t check[] = L"ABC";
	wchar_t buffer[10];

	length = mbstowcs(buffer, string, 10);
	test_assert(length != -1);
	test_assert(memcmp(buffer,check,length) == 0);
}





   

