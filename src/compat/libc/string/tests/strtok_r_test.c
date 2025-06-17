/**
 * @file
 *
 * @date Aug 09, 2020
 * @author: Rudraksh Kashyap
 */

#include <string.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("string tokenazer (strtok_r)");

TEST_CASE("parse single word string") {
	char *tok;
	char *rest;
	char str[] = "test";

	tok = strtok_r(str, " ", &rest);

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_null(strtok_r(NULL, " ", &rest));
}

TEST_CASE("parse simple string") {
	char *tok;
	char *rest;
	char str[] = "test string";

	tok = strtok_r(str, " ", &rest);

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_not_null(tok = strtok_r(NULL, " ", &rest));
	test_assert_zero(strcmp(tok, "string"));

	test_assert_null(strtok_r(NULL, " ", &rest));
}

TEST_CASE("parse string with several separators") {
	char *tok;
	char *rest;
	char str[] = "test , string";

	tok = strtok_r(str, " ", &rest);

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_not_null(tok = strtok_r(NULL, ", ", &rest));
	test_assert_zero(strcmp(tok, "string"));

	test_assert_null(strtok_r(NULL, ", ", &rest));
}
