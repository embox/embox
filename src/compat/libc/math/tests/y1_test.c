#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("y1() tests");

TEST_CASE("Test for y1() with negative argument") {
	test_assert(isnan(y1(-1.0)));
}

TEST_CASE("Test for y1(0.0)") {
	test_assert(y1(0.0) == -INFINITY);
}

TEST_CASE("Test for y1(INFINITY)") {
	test_assert(y1(INFINITY) == 0.0);
}

TEST_CASE("Test for y1(NaN)") {
	test_assert(isnan(y1(NAN)));
}

TEST_CASE("Test for y1(1.0)") {
	test_assert(fabs(y1(1.0) + 0.781212) < EPS);
}

TEST_CASE("Test for y1(2.0)") {
	test_assert(fabs(y1(2.0) + 0.107032) < EPS);
}

TEST_CASE("Test for y1(0.5)") {
	test_assert(fabs(y1(0.5) + 1.47147) < EPS);
}