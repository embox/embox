/**
 * @file
 * @brief
 *
 * @author  Nastya Nizharadze
 * @date    08.07.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strcoll test");

TEST_CASE("strcoll should return a diff of the first not matched chars") {
	char *line1;
	char *line2;

	line1 = "abA";
	line2 = "aba";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = ".ab";
	line2 = "ab";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = "1ab";
	line2 = "ab";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = "ab";
	line2 = "ab";
	test_assert_zero(strcoll(line1, line2));

	line1 = "ab";
	line2 = "abc";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = "1";
	line2 = "10";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = "2";
	line2 = "10";
	test_assert_true(strcoll(line1, line2) > 0);

	line1 = "2";
	line2 = "30";
	test_assert_true(strcoll(line1, line2) < 0);

	line1 = "11";
	line2 = "20";
	test_assert_true(strcoll(line1, line2) < 0);
}
