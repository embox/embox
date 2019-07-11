/**
 * @file
 * @brief
 *
 * @author  Nastya Nizharadze
 * @date    08.07.2019
 */

#include <stdlib.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strcoll test");

TEST_CASE("strcoll should return negative number because 'A' < 'a'") {
	char *line1;
	char *line2;
	line1 = "abA";
	line2 = "aba";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return negative number because '.' < 'a'") {
	char *line1;
	char *line2;
	line1 = ".ab";
	line2 = "ab";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return negative number because '1' < 'a'") {
	char *line1;
	char *line2;
	line1 = "1ab";
	line2 = "ab";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return 0 because lines are equal") {
	char *line1;
	char *line2;
	line1 = "ab";
	line2 = "ab";
	test_assert_zero(strcoll(line1, line2));
}

TEST_CASE("strcoll should return negative number because '\\0' < 'a'") {
	char *line1;
	char *line2;
	line1 = "ab";
	line2 = "abc";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return negative number because '\\0' < '1'") {
	char *line1;
	char *line2;
	line1 = "1";
	line2 = "10";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return positive number because '2' > '1'") {
	char *line1;
	char *line2;
	line1 = "2";
	line2 = "10";
	test_assert_true(strcoll(line1, line2) > 0);
}

TEST_CASE("strcoll should return negative number because '2' < '3'") {
	char *line1;
	char *line2;
	line1 = "2";
	line2 = "30";
	test_assert_true(strcoll(line1, line2) < 0);
}

TEST_CASE("strcoll should return negative number because '1' < '2'") {
	char *line1;
	char *line2;
	line1 = "11";
	line2 = "20";
	test_assert_true(strcoll(line1, line2) < 0);
}
