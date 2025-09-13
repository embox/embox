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
