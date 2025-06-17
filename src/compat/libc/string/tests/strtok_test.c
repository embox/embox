/**
 * @file
 *
 * @date Aug 26, 2013
 * @author: Anton Bondarev
 */

#include <string.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("string tokenazer (strtok)");

TEST_CASE("parse single word string") {
	char *tok;
	char str[] = "test";

	tok = strtok(str, " ");

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_null(strtok(NULL, " "));
}

TEST_CASE("parse simple string") {
	char *tok;
	char str[] = "test string";

	tok = strtok(str, " ");

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_not_null(tok = strtok(NULL, " "));
	test_assert_zero(strcmp(tok, "string"));

	test_assert_null(strtok(NULL, " "));
}

TEST_CASE("parse string with several separators") {
	char *tok;
	char str[] = "test , string";

	tok = strtok(str, ", ");

	test_assert_not_null(tok);

	test_assert_zero(strcmp(tok, "test"));

	test_assert_not_null(tok = strtok(NULL, ", "));
	test_assert_zero(strcmp(tok, "string"));

	test_assert_null(strtok(NULL, ", "));
}
