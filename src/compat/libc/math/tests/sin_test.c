#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("sin() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) < 1e-9;
}

TEST_CASE("Test for sin(0)") {
	test_assert(is_close(sin(0), 0));
}

TEST_CASE("Test for sin(Pi/6)") {
	test_assert(is_close(sin(M_PI / 6), 0.5));
}

TEST_CASE("Test for sin(Pi/2)") {
	test_assert(is_close(sin(M_PI / 2), 1));
}

TEST_CASE("Test for sin(Pi)") {
	test_assert(is_close(sin(M_PI), 0));
}

TEST_CASE("Test for sin(0)") {
	test_assert(is_close(sin(-0), 0));
}

TEST_CASE("Test for sin(+INFINITY)") {
	test_assert(isnan(sin(INFINITY)));
}

TEST_CASE("Test for sin(NAN)") {
	test_assert(isnan(sin(NAN)));
}
