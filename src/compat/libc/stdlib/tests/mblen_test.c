/**
 * @file
 * @brief Test unit for stdlib/mblen.
 *
 * @date Dec 7, 2018
 * @author Chubukov Filipp
 */

#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_STRING "ABC"
EMBOX_TEST_SUITE("stdlib/mblen_test");

TEST_CASE("check char") {
	char test_string[] = TEST_STRING;
	test_assert(mblen(test_string, sizeof(char)) == sizeof(char));
}
