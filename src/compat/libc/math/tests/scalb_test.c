/**
 * @file scalb_test.c
 *
 * @brief Tests for scalb() from math.h, as per POSIX/IEEE 754.
 *        Assumes scalb(x, n) computes x * 2^n with n truncated to int.
 *        Likely uses local_scalbn from math_private.h.
 *
 * @date May 21, 2025
 * @author ShigrafS
 */

#include <math.h>
#include <float.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("scalb() tests");

#define EPSILON 1e-10

/* Helper macro for floating-point comparison */
#define test_assert_float_equal(actual, expected, msg) \
    test_assert_msg(fabs((actual) - (expected)) < EPSILON, msg, actual)

/* Test normal values */
TEST_CASE("scalb() with positive x and n") {
    test_assert_float_equal(scalb(2.0, 3.0), 16.0, "scalb(2.0, 3.0) failed: got %f");
}

/* Test negative exponent */
TEST_CASE("scalb() with negative n") {
    test_assert_float_equal(scalb(1.0, -1.0), 0.5, "scalb(1.0, -1.0) failed: got %f");
}

/* Test non-integer exponent */
TEST_CASE("scalb() with non-integer n") {
    test_assert_float_equal(scalb(2.0, 3.7), 16.0, "scalb(2.0, 3.7) failed: got %f");
}

/* Test zero */
TEST_CASE("scalb() with zero x") {
    test_assert_float_equal(scalb(0.0, 10.0), 0.0, "scalb(0.0, 10.0) failed: got %f");
}

/* Test infinity and NaN */
TEST_CASE("scalb() with infinity and NaN") {
    test_assert(isinf(scalb(INFINITY, 10.0)), "scalb(INFINITY, 10.0) is not infinity");
    test_assert(isnan(scalb(NAN, 10.0)), "scalb(NAN, 10.0) is not NaN");
}