/**
 * @file
 *
 * @date October 09, 2024
 * @author Matvey Nazdruhin
 */

#include <math.h>

#include <embox/test.h>


EMBOX_TEST_SUITE("log() tests");

TEST_CASE("Test for log() with negative argument") {
	double x = log(-1.0);
	test_assert(isnan(x));
}

TEST_CASE("Test for log() with zero argument") {
	double x = log(0.0);
	test_assert(x == -INFINITY);
}

TEST_CASE("Test for log() with negative zero") {
	double x = log(-0.0);
	test_assert(x == -INFINITY);
}

TEST_CASE("Test for log() with +INFINITY argument") {
	double x = log(INFINITY);
	test_assert(isinf(x));
}

TEST_CASE("Test for log() with -INFINITY argument") {
	double x = log(-INFINITY);
	test_assert(isnan(x));
}

TEST_CASE("Test for log() with NaN argument") {
	double x = log(NAN);
	test_assert(isnan(x));
}

TEST_CASE("Test for log(e)") {
	double x = log(M_E);
	test_assert(x == 1.0);
}

TEST_CASE("Test for log(1.0)") {
	double x = log(1.0);
	test_assert(x == 0.0);
}

TEST_CASE("Test for log(e^2)") {
	double x = log(M_E * M_E);
	test_assert(x == 2.0);
}

TEST_CASE("Test for log(1/e)") {
	double x = log(1 / M_E);
	test_assert(x == -1.0);
}

TEST_CASE("Test for log(sqrt(e))") {
	double x = log(sqrt(M_E));
	test_assert(x == 0.5);
}