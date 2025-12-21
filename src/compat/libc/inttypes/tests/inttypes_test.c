/**
 * @file
 * @brief
 *
 * @date 23.08.13
 * @author Alexander Kalmuk
 */

#include <inttypes.h>
#include <limits.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("testing functions from inttypes.h");

TEST_CASE("imaxabs") {
	test_assert_equal(imaxabs(INTMAX_MIN + 1), INTMAX_MAX);
}

TEST_CASE("imaxdiv") {
	intmax_t numer = INTMAX_MAX;
	intmax_t denom = (INTMAX_MAX - 1) / 2;
	imaxdiv_t result;

	result = imaxdiv(numer, denom);

	test_assert_equal(result.quot, 2);
	test_assert_equal(result.rem,  1);
}

TEST_CASE("strtoimax") {
	char *str = "9223372036854775807";
	intmax_t num;

	num = strtoimax(str, NULL, 10);

	test_assert_equal(num, INTMAX_MAX);
}

TEST_CASE("strtoumax") {
	char *str = "18446744073709551615";
	uintmax_t num;

	num = strtoumax(str, NULL, 10);

	test_assert_equal(num, UINTMAX_MAX);
}

TEST_CASE("wcstoimax") {
	wchar_t *wstr = L"9223372036854775807";
	intmax_t num;

	num = wcstoimax(wstr, NULL, 10);

	test_assert_equal(num, INTMAX_MAX);
}

TEST_CASE("wcstoumax") {
	wchar_t *wstr = L"18446744073709551615";
	uintmax_t num;

	num = wcstoumax(wstr, NULL, 10);

	test_assert_equal(num, UINTMAX_MAX);
}
