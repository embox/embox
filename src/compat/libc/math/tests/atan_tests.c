/**
 * @file
 *
 * @date May 20, 2024
 * @author liu kang bing
 */

#include <math.h>

#include <embox/test.h>
EMBOX_TEST_SUITE("atan() tests");

static bool is_equal(double x, double y) {
	return fabs(x - y) < 1e-9;
}

TEST_CASE("Test for simple value") {
	test_assert(is_equal(atan(0.0), 0.0));
	test_assert(is_equal(atan(1.0), M_PI_4));
	test_assert(is_equal(atan(-1.0), -M_PI_4));
}

TEST_CASE("Test for atan(tan(x)) == x") {
	double lower = -M_PI_2;
	double uper = M_PI_2;
	double step = 1e-3;
	for (double x = lower; x < uper; x += step) {
		test_assert(is_equal(atan(tan(x)), x));
	}
}

TEST_CASE("Test for tan(atan(x)) == x") {
	double lower = -1e5;
	double uper = 1e5;
	double step = 1;
	for (double x = lower; x < uper; x += step) {
		test_assert(is_equal(tan(atan(x)), x));
	}
}

TEST_CASE("Test for corner cases and failure case") {
	test_assert(is_equal(M_PI_2, atan(INFINITY)));
	test_assert(is_equal(-M_PI_2, atan(-INFINITY)));
	//isnan(atan(NAN));
}
