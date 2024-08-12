#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("log10() tests");

TEST_CASE("Test for log10() with negative argument") {
	double x = log10(-1.0);
	test_assert(isnan(x));
}

TEST_CASE("Test for log10(0.0)") {
	double x = log10(0.0);
	test_assert(x == -INFINITY);
}

TEST_CASE("Test for log10(-0.0)") {
	double x = log10(-0.0);
	test_assert(x == -INFINITY);
}

TEST_CASE("Test for log10(0.001)") {
	double x = log10(0.001);
	test_assert(x == -3.0);
}

TEST_CASE("Test for log10(1.0)") {
	double x = log10(1.0);
	test_assert(x == 0.0);
}

TEST_CASE("Test for log10(100.0)") {
	double x = log10(100.0);
	test_assert(x == 2.0);
}

TEST_CASE("Test for log10(+INFINITY)") {
	double x = log10(INFINITY);
	test_assert(isinf(x));
}

TEST_CASE("Test for log10(-INFINITY)") {
	double x = log10(-INFINITY);
	test_assert(isnan(x));
}

TEST_CASE("Test for log10(NaN)") {
	double x = log10(NAN);
	test_assert(isnan(x));
}
