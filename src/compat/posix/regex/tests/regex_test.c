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

#if 0
TEST_CASE("compile pattern with symbol ^") {
	regex_t regex;
	int ret;

	ret = regcomp(&regex, "^a", 0);
	test_assert_zero(ret);
	regfree(&regex);
}

TEST_CASE("full string match") {
	regex_t regex;
	regmatch_t match[10];

	regcomp(&regex, "^hello$", 0);
	test_assert_zero(regexec(&regex, "hello", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "hell", 10, match, 0));
	regfree(&regex);
}


TEST_CASE("pattern with posix classes: alnum") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "^a[[:alnum:]]", 0));
	test_assert_zero(regexec(&regex, "aa", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "ba", 10, match, 0));
	regfree(&regex);
}

TEST_CASE("pattern with posix classes: digit") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "^a[[:digit:]]", 0));
	test_assert_zero(regexec(&regex, "a1", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "aa", 10, match, 0));
	regfree(&regex);
}
#endif

TEST_CASE("correct match: [] group and alphanimeric, sympol '*'") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "#[-\\w_]*", 0));
	test_assert_zero(regexec(&regex, "#ab-sF", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "ab-sF", 10, match, 0));
	regfree(&regex);
}

TEST_CASE("correct match: [] group, sympol '+'") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "Simple\\s[a-zA-Z]+\\stest", 0));
	test_assert_zero(regexec(&regex, "Simple simple test", 10, match, 0));
	test_assert_zero(regexec(&regex, "Simple xxx test", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "Simpl simple test", 10, match, 0));
	regfree(&regex);
}

TEST_CASE("correct match: {} and letter") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "\\a{2,3}", 0));
	test_assert_zero(regexec(&regex, "pp", 10, match, 0));
	test_assert_zero(regexec(&regex, "ppp", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "p", 10, match, 0));
	regfree(&regex);
}

#if 0
TEST_CASE("correct match: {} and letter") {
	regex_t regex;
	regmatch_t match[10];

	test_assert_zero(regcomp(&regex, "^\\a{2,3}$", 0));
	test_assert_zero(regexec(&regex, "pp", 10, match, 0));
	test_assert_zero(regexec(&regex, "ppp", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "p", 10, match, 0));
	test_assert_not_zero(regexec(&regex, "pppp", 10, match, 0));
	regfree(&regex);
}
#endif

TEST_CASE("regexec: several matches") {
	regex_t regex;
	regmatch_t match[10];
	int ret;

	ret = regcomp(&regex, "k", 0);
	test_assert_zero(ret);
	test_assert_zero(regexec(&regex, "kk#ab-kF", 10, match, 0));
	test_assert_true(match[2].rm_eo);
	test_assert_false(match[3].rm_eo);
	regfree(&regex);
}

TEST_CASE("regerror") {
	regex_t regex;
	char error[40] = "";

	test_assert_not_zero(regcomp(&regex, "(s$", 0));
	test_assert_zero(regerror(0, &regex, error, 40));
	test_assert_not_zero(strlen(error));

	regfree(&regex);
}
