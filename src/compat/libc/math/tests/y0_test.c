#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("y0() tests");

#define EPS 1e-5

TEST_CASE("Test for y0() with negative argument") {
	test_assert(isnan(y0(-1.0)));
}

TEST_CASE("Test for y0(0.0)") {
	test_assert(y0(0.0) == -INFINITY);
}

TEST_CASE("Test for y0(INFINITY)") {
	test_assert(fabs(y0(INFINITY)) < EPS);
}

TEST_CASE("Test for y0(NaN)") {
	test_assert(isnan(y0(NAN)));
}

TEST_CASE("Test for y0(1.0)") {
	test_assert(fabs(y0(1.0) - 0.088257) < EPS);
}

TEST_CASE("Test for y0(2.0)") {
	test_assert(fabs(y0(2.0) - 0.510376) < EPS);
}

TEST_CASE("Test for y0(0.5)") {
	test_assert(fabs(y0(0.5) + 0.444519) < EPS);
}

TEST_CASE("Test for y0(1e-10)") {
	test_assert(y0(1e-10) < 0);
}

TEST_CASE("Test for y0(0.8936)") {
	test_assert(fabs(y0(0.8936)) < 1e-4);
}

TEST_CASE("Test for y0(10.0)") {
	test_assert(fabs(y0(10.0) - 0.055671) < EPS);
}
