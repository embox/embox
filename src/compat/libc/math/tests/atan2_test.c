/**
 * @file
 *
 * @date Jun 04, 2024
 * @author adithya
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("atan2() tests");

static bool is_equal(double x, double y) {
	return fabs(x - y) < 1e-9;
}

TEST_CASE("Test for atan2() basic quadrant checks") {
	// First quadrant, vertical line
	test_assert(is_equal(atan2(1.0, 0.0), M_PI_2));
	// First quadrant, horizontal line
	test_assert(is_equal(atan2(0.0, 1.0), 0.0));
	// Fourth quadrant, vertical line
	test_assert(is_equal(atan2(-1.0, 0.0), -M_PI_2));
	// Second quadrant, horizontal line passing through negative x-axis
	test_assert(is_equal(atan2(0.0, -1.0), M_PI));
}

TEST_CASE("Test for atan2() with zero arguments") {
	// Should ideally handle as undefined behavior
	test_assert(is_equal(atan2(0.0, 0.0), 0.0));
	test_assert(is_equal(atan2(0.0, -0.0), M_PI));
	test_assert(is_equal(atan2(-0.0, 0.0), -0.0));
	test_assert(is_equal(atan2(-0.0, -0.0), -M_PI));
}

TEST_CASE("Test for atan2() with infinity") {
	// Vertical line from positive infinity
	test_assert(is_equal(atan2(INFINITY, 1.0), M_PI_2));
	// Vertical line from negative infinity
	test_assert(is_equal(atan2(-INFINITY, 1.0), -M_PI_2));
	// Horizontal line from positive infinity
	test_assert(is_equal(atan2(1.0, INFINITY), 0.0));
	// Horizontal line crossing through negative infinity
	test_assert(is_equal(atan2(1.0, -INFINITY), M_PI));
}
