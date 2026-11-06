/**
 * @file
 *
 * @date June 4, 2024
 * @author Kapel Dev
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("acos() tests");

static bool is_close(double x, double y) {
	return fabs(x - y) < 1e-9;
}

TEST_CASE("Test for acos(1.0)") {
	test_assert(acos(1.0) == 0.0);
}

TEST_CASE("Test for acos(-1.0)") {
	test_assert(is_close(acos(-1.0), M_PI));
}

TEST_CASE("Test for acos(0.0)") {
	test_assert(is_close(acos(0.0), M_PI_2));
}

TEST_CASE("Test for acos(1/2)") {
	test_assert(is_close(acos(0.5), (M_PI / 3)));
}

TEST_CASE("Test for acos(-1/2)") {
	test_assert(is_close(acos(-0.5), ((2 * M_PI) / 3)));
}

TEST_CASE("Test for acos(+INFINITY)") {
	test_assert(isnan(acos(INFINITY)));
}

TEST_CASE("Test for acos(-INFINITY)") {
	test_assert(isnan(acos(-INFINITY)));
}

TEST_CASE("Test for acos(NAN)") {
	test_assert(isnan(acos(NAN)));
}
