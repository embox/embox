/**
 * @file
 *
 * @date October 10, 2024
 * @author SecretPersona5 (Nafikov Ruslan)
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("modf() tests");

int double_equals(double a, double b, double epsilon) {
	return fabs(a - b) < epsilon;
}

TEST_CASE("Test for modf(123.456)") {
	double int_part;
	double frac_part = modf(123.456, &int_part);

	double epsilon = 1e-9;

	test_assert(double_equals(int_part, 123.0, epsilon));
	test_assert(double_equals(frac_part, 0.456, epsilon));
}

TEST_CASE("Test for modf(-123.456)") {
	double int_part;
	double frac_part = modf(-123.456, &int_part);

	double epsilon = 1e-9;

	test_assert(double_equals(int_part, -123.0, epsilon));
	test_assert(double_equals(frac_part, -0.456, epsilon));
}

TEST_CASE("Test for modf(0.456)") {
	double int_part;
	double frac_part = modf(0.456, &int_part);
	test_assert(int_part == 0.0);
	test_assert(frac_part == 0.456);
}

TEST_CASE("Test for modf(-0.456)") {
	double int_part;
	double frac_part = modf(-0.456, &int_part);
	test_assert(int_part == -0.0);
	test_assert(frac_part == -0.456);
}

TEST_CASE("Test for modf(123.0)") {
	double int_part;
	double frac_part = modf(123.0, &int_part);
	test_assert(int_part == 123.0);
	test_assert(frac_part == 0.0);
}

TEST_CASE("Test for modf(-123.0)") {
	double int_part;
	double frac_part = modf(-123.0, &int_part);
	test_assert(int_part == -123.0);
	test_assert(frac_part == 0.0);
}

TEST_CASE("Test for modf(0.0)") {
	double int_part;
	double frac_part = modf(0.0, &int_part);
	test_assert(int_part == 0.0);
	test_assert(frac_part == 0.0);
}

TEST_CASE("Test for modf(-0.0)") {
	double int_part;
	double frac_part = modf(-0.0, &int_part);
	test_assert(int_part == -0.0);
	test_assert(frac_part == -0.0);
}

TEST_CASE("Test for modf(+INFINITY)") {
	double int_part;
	double frac_part = modf(INFINITY, &int_part);
	test_assert(isinf(int_part) && int_part > 0);
	test_assert(frac_part == 0.0);
}

TEST_CASE("Test for modf(-INFINITY)") {
	double int_part;
	double frac_part = modf(-INFINITY, &int_part);
	test_assert(isinf(int_part) && int_part < 0);
	test_assert(frac_part == 0.0);
}