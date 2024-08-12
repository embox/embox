/**
 * @file
 *
 * @date June 4, 2024
 * @author Kapel Dev
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("acos() tests");

TEST_CASE("Test for acos() with negative argument") {
	double x = acos(-1.0);
	double y = acos(-1.0) - acos(1.0);
	test_assert(x == y);
}

TEST_CASE("Test for acos(1.0)") {
	double x = acos(1.0);
	test_assert(x == 0.0);
}

TEST_CASE("Test for acos(0.0)") {
	double x = acos(0.0);
	double y = acos(-1.0);
	test_assert(x == (y / 2));
}
