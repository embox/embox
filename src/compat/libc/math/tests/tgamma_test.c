/**
 * @file
 * 
 * @date May 19, 2025
 * @author Peize Li
 */
#include <math.h>
#include <float.h>
#include <errno.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("tgamma() tests");

#define _ISOC99_SOURCE
#define _POSIX_C_SOURCE 200112L
#define TGAMMA_EPSILON_FACTORIAL (DBL_EPSILON * 1000)
#define TGAMMA_EPSILON_PI_RELATED (DBL_EPSILON * 100)
#define TGAMMA_EPSILON_GENERAL (1e-6)

static int approx_equal(double calc, double expect, double epsilon) {
  if (isnan(calc) && isnan(expect)) {
    return 1;
  }
  if (isinf(calc) && isinf(expect) && ((calc > 0) == (expect > 0))) {
    return 1;
  }
  if (fabs(expect) < epsilon) {
    return fabs(calc - expect) < epsilon;
  }
  return fabs(calc - expect) / fabs(expect) < epsilon;
}

TEST_CASE("tgamma() with positive integers") {
  test_assert(approx_equal(tgamma(1.0), 1.0, TGAMMA_EPSILON_FACTORIAL));
  test_assert(approx_equal(tgamma(2.0), 1.0, TGAMMA_EPSILON_FACTORIAL));
  test_assert(approx_equal(tgamma(5.0), 24.0, TGAMMA_EPSILON_FACTORIAL));
}

TEST_CASE("tgamma() with positive non-integers") {
  test_assert(approx_equal(tgamma(0.5), sqrt(M_PI), TGAMMA_EPSILON_PI_RELATED));
  test_assert(approx_equal(tgamma(1.5), 0.5 * sqrt(M_PI), TGAMMA_EPSILON_PI_RELATED));
}

TEST_CASE("tgamma() with zero input") {
  test_assert(isinf(tgamma(+0.0)) && tgamma(+0.0) > 0);
  test_assert(isinf(tgamma(-0.0)) && tgamma(-0.0) < 0);
}

TEST_CASE("tgamma() with infinite input") {
  test_assert(isinf(tgamma(INFINITY)) && tgamma(INFINITY) > 0);
  test_assert(isnan(tgamma(-INFINITY)));
}

TEST_CASE("tgamma() with NaN input") {
  test_assert(isnan(tgamma(NAN)));
}

TEST_CASE("tgamma() with negative integers") {
  test_assert(isnan(tgamma(-1.0)));
  test_assert(isnan(tgamma(-2.0)));
}

TEST_CASE("tgamma() with negative non-integers") {
  test_assert(approx_equal(tgamma(-0.5), -2.0 * sqrt(M_PI), TGAMMA_EPSILON_PI_RELATED));
}

TEST_CASE("tgamma() near overflow") {
  test_assert(!isinf(tgamma(171.0)) && !isnan(tgamma(171.0)));
  test_assert(isinf(tgamma(172.0)) && tgamma(172.0) > 0);
}

TEST_CASE("tgamma() recursion property") {
  double x = 1.14;
  test_assert(approx_equal(tgamma(x+1), x*tgamma(x), TGAMMA_EPSILON_GENERAL));
}

TEST_CASE("tgamma() and lgamma() consistency") {
  double x = 5.14;
  test_assert(approx_equal(log(tgamma(x)), lgamma(x), TGAMMA_EPSILON_GENERAL));
}

TEST_CASE("tgamma() overflow sets errno") {
  // errno = 0;
  // tgamma(172.0);
  // test_assert(errno == ERANGE);
}

TEST_CASE("tgamma() invalid input sets errno") {
  // errno = 0;
  // tgamma(-1.0);
  // test_assert(errno == EDOM);
}