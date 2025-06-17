/**
 * @file
 * @brief Tests for strpbrk().
 *
 * @date Mar 30, 2020
 * @author: Neeraj Adhikari
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("strpbrk test suite");

TEST_CASE("strpbrk with empty strings") {
	char nonempty[] = "test";
	char empty[] = "";

	test_assert_null(strpbrk(nonempty, empty));
	test_assert_null(strpbrk(empty, nonempty));
	test_assert_null(strpbrk(empty, empty));
}

TEST_CASE("strpbrk with printable strings") {
	char embox[] = "EMBOXembox";
	char vowels_lower[] = "aeiou";
	char digits[] = "0123456789";
	char symbols[] = "~!@#$%^&*()_+?><:}{\"'";
	char nine[] = "9";
	char hash[] = "#";

	test_assert_equal(strpbrk(embox, embox), embox);
	test_assert_equal(strpbrk(embox, vowels_lower), embox + 5);
	test_assert_equal(strpbrk(vowels_lower, embox), vowels_lower + 1);
	test_assert_equal(strpbrk(digits, nine), digits + 9);
	test_assert_null(strpbrk(symbols, nine));
	test_assert_equal(strpbrk(symbols, hash), symbols + 3);
}

TEST_CASE("strpbrk with non-printable data") {
	char data[] = "\xCA\xFE\xBA\xBE";
	char key[] = "\xBA";

	test_assert_equal(strpbrk(data, key), data + 2);
	test_assert_equal(strpbrk(key, data), key);
}
