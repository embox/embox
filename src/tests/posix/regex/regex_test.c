/**
 * @file
 *
 * @date Jun 13, 2014
 * @author Anton Bondarev
 */

#include <errno.h>

#include <regex.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("regex");


TEST_CASE(" The structure type regex_t contains at least the following member:"
		"size_t    re_nsub") {
	regex_t regex;

	regex.re_nsub = 0;

	test_assert_zero(regex.re_nsub);
}

TEST_CASE("match") {
	regex_t regex;
	int ret;

	ret = regcomp(&regex, "^a[[:alnum:]]", 0);
	test_assert_not_zero(ret);
}

TEST_CASE("compile and match") {
	regex_t regex;
	regmatch_t match[1];
	int ret;

	ret = regcomp(&regex, "k", 0);
	test_assert_zero(ret);
	test_assert_zero(regexec(&regex, "k", 1, match, 0));
	regfree(&regex);
	test_assert_equal(regexec(&regex, "k", 1, match, 0), -EINVAL);
}

TEST_CASE("match") {
	regex_t regex;
	regmatch_t match[10];
	int ret;

	ret = regcomp(&regex, "k", 0);
	test_assert_zero(ret);
	test_assert_zero(regexec(&regex, "kk#ab-kF", 10, match, 0));
	regfree(&regex);
}

TEST_CASE("error") {
	regex_t regex;
	char error[40] = "";

	test_assert_not_zero(regcomp(&regex, "(s$", 0));
	test_assert_zero(regerror(0, &regex, error, 40));
	test_assert_not_zero(strlen(error));

	regfree(&regex);
}
