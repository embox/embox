/**
 * @file
 * @brief Test of the strstr() function.
 *
 * @date Mar 27, 2020
 * @author: Vaibhav Shinde
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strstr test suite");

TEST_CASE("strstr with no needle in the haystack (no match)") {
	test_assert_zero(strstr("the quick brown fox", "wolf"));
	test_assert_zero(strstr("the slow green turtle", "hare"));

}

TEST_CASE("strstr with multiple needles in the haystack, returns pointer of first occurrence") {
	char *multiple_words_quick = "the quick brown fox ran quicker than a quick hare";
	char *multiple_words_slow = "the turtle was slower than a slow snail";

	test_assert(strstr(multiple_words_quick, "quick") == multiple_words_quick + 4);
	test_assert(strstr(multiple_words_slow, "slow") == multiple_words_slow + 15);
}

TEST_CASE("strstr with a needle in the haystack (match of first occurrence)") {
	test_assert(strstr("the quick brown fox", "fox") != 0);
	test_assert(strstr("the slow green turtle", "turtle") != 0);
}

