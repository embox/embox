/**
 * @file
 * @brief Test of the strcasestr() function.
 *
 * @date Mar 27, 2020
 * @author: Vaibhav Shinde
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strcasestr test suite");

TEST_CASE("strcasestr with no needle in the haystack (no match)") {
	test_assert_zero(strcasestr("the quick brown fox", "wolf"));
	test_assert_zero(strcasestr("the slow green turtle", "hare"));
}

TEST_CASE("strcasestr with multiple needles in the haystack, returns pointer of first occurrence") {
	char *multiple_words_quick = "the QuiCk brown fox ran qUIcker than a quIcK hare";
	char *multiple_words_slow = "the turtle was sLoWer than a SLOW snail";

	test_assert(strcasestr(multiple_words_quick, "quick") == multiple_words_quick + 4);
	test_assert(strcasestr(multiple_words_slow, "slow") == multiple_words_slow + 15);
}

TEST_CASE("strcasestr with a needle with different cases in the haystack (match of first occurrence)") {
	test_assert(strcasestr("the quick brown fox", "FoX") != 0);
	test_assert(strcasestr("the slow green turtle", "tuRtLe") != 0);
}

