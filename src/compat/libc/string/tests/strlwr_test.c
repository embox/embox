/**
 * @file
 *
 * @date Mar 21, 2020
 * @author: Faisal Riyaz
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suit for strlwr()");

TEST_CASE("strlwr with uppercase string") {
	char str[] = "STRING";

	test_assert_zero(strcmp("string", strlwr(str)));	
}

TEST_CASE("strlwr with mixed string") {
	char str1[] = "ABC string 123";
	char str2[] = "ABCdefGH";	

	test_assert_zero(strcmp("abc string 123", strlwr(str1)));
	test_assert_zero(strcmp("abcdefgh", strlwr(str2)));
}

TEST_CASE("checking strlwr result with differnt string") {
	char str[] = "STRING";

	test_assert_not_zero(strcmp("abcdef", strlwr(str)));
	test_assert_not_zero(strcmp("STriNG", strlwr(str)));
}

TEST_CASE("checking strlwr result with string of differnt length") {
	char str[] = "STRING";

	test_assert_not_zero(strcmp("strin", strlwr(str)));
	test_assert_not_zero(strcmp("stringg", strlwr(str)));	
}
