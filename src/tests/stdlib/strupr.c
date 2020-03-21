/**
 * @file
 *
 * @date Mar 21, 2020
 * @author: Faisal Riyaz
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for strupr()");

TEST_CASE("strupr with lowercase string") {
	char str[] = "string";

	test_assert_zero(strcmp("STRING", strupr(str)));
}

TEST_CASE("strupr with mixed string") {
	char str1[] = "ABC string 123";
	char str2[] = "abcDEFgh";

	test_assert_zero(strcmp("ABC STRING 123", strupr(str1)));
	test_assert_zero(strcmp("ABCDEFGH", strupr(str2)));	
}

TEST_CASE("checking strupr result with differnt string") {
	char str[] = "string";

	test_assert_not_zero(strcmp("ABCDEF", strupr(str)));
	test_assert_not_zero(strcmp("STriNG", strupr(str)));
}

TEST_CASE("checking strupr result with differnt length") {
	char str[] = "string";

	test_assert_not_zero(strcmp("STRIN", strupr(str)));
	test_assert_not_zero(strcmp("STRINGG", strupr(str)));	
}
