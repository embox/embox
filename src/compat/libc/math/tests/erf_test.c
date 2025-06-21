/**
 * @file erf_test.c
 *
 * @brief Tests for erf() from math.h, as per POSIX/IEEE 754.
 *        Computes the error function of x, defined as erf(x) = 2/sqrt(pi) * integral from 0 to x of exp(-t^2) dt.
 *
 * @date June 21, 2025
 * @author ShigrafS
 */

#include <math.h>
#include <float.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("erf() tests");

#define EPSILON 1e-10

/* Helper macro for floating-point comparison */
#define test_assert_float_equal(actual, expected, msg) \
    test_assert_msg(fabs((actual) - (expected)) < EPSILON, msg, actual)

/* Test normal values */
TEST_CASE("erf() with positive x") {
    test_assert_float_equal(erf(1.0), 0.8427007929497149, "erf(1.0) failed: got %f");
}

TEST_CASE("erf() with negative x") {
    test_assert_float_equal(erf(-1.0), -0.8427007929497149, "erf(-1.0) failed: got %f");
}

TEST_CASE("erf() with zero") {
    test_assert_float_equal(erf(0.0), 0.0, "erf(0.0) failed: got %f");
}

TEST_CASE("erf() with small x") {
    test_assert_float_equal(erf(0.1), 0.1124629160182849, "erf(0.1) failed: got %f");
}

TEST_CASE("erf() with large x") {
    test_assert_float_equal(erf(5.0), 0.9999999845827421, "erf(5.0) failed: got %f");
}

TEST_CASE("erf() with infinity and NaN") {
    test_assert_float_equal(erf(INFINITY), 1.0, "erf(INFINITY) failed: got %f");
    test_assert_float_equal(erf(-INFINITY), -1.0, "erf(-INFINITY) failed: got %f");
    test_assert(isnan(erf(NAN)), "erf(NAN) is not NaN");
}

/* Test overflow or error conditions (erf() typically doesn't overflow, but check errno) */
TEST_CASE("erf() with large input and errno") {
    errno = 0;
    double result = erf(DBL_MAX);
    test_assert_float_equal(result, 1.0, "erf(DBL_MAX) failed: got %f");
    test_assert(errno == 0, "erf(DBL_MAX) incorrectly set errno");
}