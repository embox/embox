/**
 * @file
 *
 * @date June 5, 2024
 * @author loharmurtaza
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("asin() tests");

TEST_CASE("Test for asin(1.0)") {
	double x = asin(1.0);
	test_assert(x == M_PI_2);
}

TEST_CASE("Test for asin(0.0)") {
	double x = asin(0.0);
	test_assert(x == 0.0);
}

TEST_CASE("Test for asin(-1.0)") {
	double x = asin(-1.0);
	test_assert(x == -M_PI_2);
}
