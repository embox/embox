/**
 * @file
 * 
 * @date December 18, 2025
 * @author Biancaa Ramesh
 */

#include <math.h>
#include <float.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("erf() tests");

static int close_eps(double a, double b, double eps) {
	double d = a - b;
	if (d < 0) d = -d;
	return d <= eps;
}


TEST_CASE("erf(0.0) == 0.0") {
	test_assert(erf(0.0) == 0.0);
}

TEST_CASE("erf(-0.0) preserves the sign of -0.0") {
	double v = erf(-0.0);
	test_assert(v == -0.0);
	test_assert(signbit(v));
}

TEST_CASE("erf(NaN) is NaN") {
	test_assert(isnan(erf(NAN)));
}

TEST_CASE("erf(+INFINITY) == 1.0") {
	test_assert(erf(INFINITY) == 1.0);
}

TEST_CASE("erf(-INFINITY) == -1.0") {
	test_assert(erf(-INFINITY) == -1.0);
}

TEST_CASE("odd: erf(-x) == -erf(x) for x=0.5") {
	double x = 0.5;
	test_assert(close_eps(erf(-x), -erf(x), 1e-12));
}

TEST_CASE("erf(1.0) ~= 0.842700792949... (error <= 1e-6)") {
	double ref = 0.84270079294971486934;
	test_assert(close_eps(erf(1.0), ref, 1e-6));
}

TEST_CASE("erf(x) is monotonic increasing") {
	test_assert(erf(0.5) < erf(1.0));
	test_assert(erf(1.0) < erf(2.0));
}

TEST_CASE("erf saturates near 1") {
	test_assert(erf(5.0) > 0.999999);
	test_assert(erf(-5.0) < -0.999999);
}

TEST_CASE("erf(x) ~= 2/sqrt(pi)*x for small x") {
	double x = 1e-10;
	double approx = 1.1283791670955126 * x; /* 2/sqrt(pi) */
	test_assert(close_eps(erf(x), approx, 1e-20));
}
