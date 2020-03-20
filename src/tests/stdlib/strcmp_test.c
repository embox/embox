/**
 * @file
 * @brief Test of strcmp() function.
 *
 * @date 21.03.20
 * @autor Md Sahil
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strcmp testing suite");

TEST_CASE("strcmp should be able to recognize equal strings as equals") {
	char *a1 = "abc";
	char *a2 = "abc";
	char *b1 = "abcxyz";
	char *b2 = "abcxyz";
	char *c1 = "";
	char *c2 = "";

	test_assert_zero(strcmp(a1, a2));
	test_assert_zero(strcmp(b1, b2));
	test_assert_zero(strcmp(c1, c2));
}

TEST_CASE("strcmp should be able to recognize unequal strings as unequals") {
	char *a1 = "abc";
	char *a2 = "abd";
	char *b1 = "xyzabc";
	char *b2 = "abcxyz";
	char *c1 = "abc";
	char *c2 = "";
	char *d1 = "abc";
	char *d2 = "abcd";

	test_assert(strcmp(a1, a2) < 0);
	test_assert(strcmp(b1, b2) > 0);
	test_assert(strcmp(c1, c2) > 0);
	test_assert(strcmp(d1, d2) < 0);
}

TEST_CASE("strncmp should be able to recognize equal strings as equals") {
	char *a1 = "abc";
	char *a2 = "abc";
	char *b1 = "";
	char *b2 = "";
	char *c1 = "abcdef";
	char *c2 = "abcxyz";

	test_assert_zero(strncmp(a1, a2, 2));
	test_assert_zero(strncmp(a1, a2, 3));
	test_assert_zero(strncmp(a1, a2, 5));
	test_assert_zero(strncmp(b1, b2, 1));
	test_assert_zero(strncmp(b1, b2, 3));
	test_assert_zero(strncmp(c1, c2, 1));
	test_assert_zero(strncmp(c1, c2, 3));
}

TEST_CASE("strncmp should be able to recognize unequal strings as unequals") {
	char *a1 = "abc";
	char *a2 = "abcd";
	char *b1 = "abc";
	char *b2 = "";
	char *c1 = "abcdef";
	char *c2 = "abcxyz";

	test_assert(strncmp(a1, a2, 4) < 0);
	test_assert(strncmp(a1, a2, 6) < 0);
	test_assert(strncmp(b1, b2, 1) > 0);
	test_assert(strncmp(b1, b2, 3) > 0);
	test_assert(strncmp(c1, c2, 4) < 0);
	test_assert(strncmp(c1, c2, 8) < 0);
}
