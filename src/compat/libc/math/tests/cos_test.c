#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("cos() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) < 1e-9;
}

TEST_CASE("Test for cos(0)") {
	test_assert(is_close(cos(0), 1));
}

TEST_CASE("Test for cos(Pi/3)") {
	test_assert(is_close(cos(M_PI / 3), 0.5));
}

TEST_CASE("Test for cos(Pi/2)") {
	test_assert(is_close(cos(M_PI / 2), 0.0));
}

TEST_CASE("Test for cos(Pi)") {
	test_assert(is_close(cos(M_PI), -1));
}

TEST_CASE("Test for cos(-0)") {
	test_assert(is_close(cos(-0), 1));
}

TEST_CASE("Test for cos(+INFINITY)") {
	test_assert(isnan(cos(INFINITY)));
}

TEST_CASE("Test for cos(NAN)") {
	test_assert(isnan(cos(NAN)));
}
