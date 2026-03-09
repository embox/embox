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

TEST_CASE("erf(-1.0) ~= -0.842700792949") {
    double ref = -0.84270079294971486934;
    test_assert(close_eps(erf(-1.0), ref, 1e-12));
}

TEST_CASE("erf(0.5) ~= 0.520499877813") {
    double ref = 0.52049987781304653768;
    test_assert(close_eps(erf(0.5), ref, 1e-12));
}

TEST_CASE("erf(2.0) ~= 0.995322265018") {
    double ref = 0.99532226501895273416;
    test_assert(close_eps(erf(2.0), ref, 1e-12));
}

TEST_CASE("strictly increasing: erf(x) is monotonic") {
	test_assert(erf(0.1) < erf(0.5));
	test_assert(erf(0.5) < erf(1.5));
	test_assert(erf(1.5) < erf(3.0));
}

TEST_CASE("saturation: erf(6.0) == 1.0 in double precision") {
	test_assert(erf(6.0) == 1.0);
}

TEST_CASE("saturation: erf(-6.0) == -1.0 in double precision") {
	test_assert(erf(-6.0) == -1.0);
}

TEST_CASE("linear approximation near zero: erf(1e-5) ~= 2*x/sqrt(PI)") {
	double x = 1e-5;
	double ref = 1.12837916709551257390 * x;
	test_assert(close_eps(erf(x), ref, 1e-15));
}
