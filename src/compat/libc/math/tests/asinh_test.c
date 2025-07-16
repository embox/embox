/**
 * @file
 *
 * @date February 14, 2025
 * @author Damir Yunusov
 */

#include <math.h>
#include <stdio.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("asinh() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) <= 1e-9 * fmin(fabs(x), fabs(y));
}

TEST_CASE("Test for asinh(0.0)") {
	test_assert(asinh(0.0) == 0.0);
}

TEST_CASE("Test for asinh(1.0)") {
	test_assert(is_close(asinh(1), 0.88137358702));
}

TEST_CASE("Test for asinh(-2.0)") {
	test_assert(is_close(asinh(-2), -1.443635475179));
}

TEST_CASE("Test for asinh(M_E)") {
	test_assert(is_close(asinh(M_E), 1.725382558852));
}

TEST_CASE("Test for asinh parity") {
	test_assert(is_close(-asinh(7.0), asinh(-7.0)));
}

TEST_CASE("Test for asinh(INFINITY)") {
	test_assert(isinf(asinh(INFINITY)));
}

TEST_CASE("Test for asinh(NAN)") {
	test_assert(isnan(asinh(NAN)));
}
