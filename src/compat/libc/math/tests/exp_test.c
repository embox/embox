/**
 * @file
 * 
 * @date December 18, 2025
 * @author Biancaa Ramesh
 */

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("exp() tests");

TEST_CASE("Test for exp(0.0)") {
	test_assert(exp(0.0) == 1.0);
}

TEST_CASE("Test for exp(-0.0)") {
	test_assert(exp(-0.0) == 1.0);
}

TEST_CASE("Test for exp(+INFINITY)") {
	test_assert(isinf(exp(INFINITY)));
}

TEST_CASE("Test for exp(NaN)") {
	test_assert(isnan(exp(NAN)));
}

TEST_CASE("exp(-INFINITY) == 0.0") {
	double r = exp(-INFINITY);
	test_assert(r == 0.0);
	test_assert(!signbit(r));
}

TEST_CASE("exp(1.0) ~= e") {
	double ref = 2.718281828459045;
	test_assert(close_eps(exp(1.0), ref, 1e-12));
}

TEST_CASE("exp(-1.0) ~= 1/e") {
	double ref = 0.36787944117144233;
	test_assert(close_eps(exp(-1.0), ref, 1e-12));
}

TEST_CASE("exp is monotonic") {
	test_assert(exp(1.0) > exp(0.0));
	test_assert(exp(0.0) > exp(-1.0));
}

TEST_CASE("exp underflow produces +0.0") {
	double r = exp(-1000.0);
	test_assert(r == 0.0);
	test_assert(!signbit(r));
}

TEST_CASE("exp overflow produces +INFINITY") {
	test_assert(isinf(exp(1000.0)));
}
