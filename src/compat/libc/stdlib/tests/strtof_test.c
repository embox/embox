/**
 * @file
 * @brief Test unit for stdlib/strtof.
 *
 * @date March 30, 2026
 * @author Sahil Apage
 */

#include <embox/test.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define EPS 1e-5f
#define FLOAT_EQ(a, b) (fabsf((a) - (b)) < EPS)

EMBOX_TEST_SUITE("stdlib/strtof");

TEST_CASE("Simple positive number") {
	float res = strtof("123.45", NULL);
	test_assert(FLOAT_EQ(res, 123.45f));
}

TEST_CASE("Simple negative number") {
	float res = strtof("-56.78", NULL);
	test_assert(FLOAT_EQ(res, -56.78f));
}

TEST_CASE("Leading spaces") {
	float res = strtof("   42.0", NULL);
	test_assert(FLOAT_EQ(res, 42.0f));
}

TEST_CASE("Trailing characters") {
	char *end;
	float res = strtof("12.34abc", &end);
	test_assert(FLOAT_EQ(res, 12.34f));
	test_assert(*end == 'a');
}

TEST_CASE("Invalid input") {
	char *end;
	float res = strtof("abc", &end);
	test_assert(res == 0.0f);
	test_assert(end != NULL && *end == 'a');
}

TEST_CASE("Scientific notation lowercase e") {
	float res = strtof("1.2e3", NULL);
	test_assert(FLOAT_EQ(res, 1200.0f));
}

TEST_CASE("Scientific notation uppercase E") {
	float res = strtof("2.5E2", NULL);
	test_assert(FLOAT_EQ(res, 250.0f));
}

TEST_CASE("Negative exponent") {
	float res = strtof("1e-2", NULL);
	test_assert(FLOAT_EQ(res, 0.01f));
}

TEST_CASE("Plus sign prefix") {
	float res = strtof("+99.5", NULL);
	test_assert(FLOAT_EQ(res, 99.5f));
}

TEST_CASE("Zero") {
	float res = strtof("0.0", NULL);
	test_assert(FLOAT_EQ(res, 0.0f));
}

TEST_CASE("Integer string") {
	float res = strtof("100", NULL);
	test_assert(FLOAT_EQ(res, 100.0f));
}

TEST_CASE("Empty string") {
	char *end;
	float res = strtof("", &end);
	test_assert(res == 0.0f);
	test_assert(end != NULL && *end == '\0');
}

TEST_CASE("Overflow") {
	errno = 0;
	float res = strtof("1e1000", NULL);
	test_assert(res == HUGE_VALF);
	test_assert(errno == ERANGE);
}

TEST_CASE("Underflow") {
	errno = 0;
	float res = strtof("1e-1000", NULL);
	test_assert(res == 0.0f || res < 1e-37f);
	test_assert(errno == ERANGE);
}

TEST_CASE("endptr points past consumed characters") {
	char *end;
	strtof("3.14rest", &end);
	test_assert(end != NULL && *end == 'r');
}