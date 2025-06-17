/**
 * @file scalbn_test.c
 * @brief scalbn(x, n) computes x * 2^n.
 *
 * @date May 21, 2025
 * @author ShigrafS
 */

#include <errno.h>
#include <float.h>
#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("scalbn() tests");

TEST_CASE("scalbn() with positive x and n") {
	test_assert_equal(scalbn(2.0, 3), 16);
}

TEST_CASE("scalbn() with negative n") {
	test_assert_equal(scalbn(1.0, -1), 0.5);
}

TEST_CASE("scalbn() with zero x") {
	test_assert_equal(scalbn(0.0, 10.0), 0);
}

TEST_CASE("scalbn() with infinity and NaN") {
	test_assert(isinf(scalbn(INFINITY, 10)));
	test_assert(isnan(scalbn(NAN, 10)));
}
