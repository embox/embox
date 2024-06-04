/**
 * @file
 *
 * @date June 4, 2024
 * @author Kapel Dev
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("acos() tests");


TEST_CASE("Test for acos() with negative argument") {
	test_assert(acos(-1.0) == (acos(-1.0) - acos(1.0)));
}

TEST_CASE("Test for acos(1.0)") {
	test_assert(acos(1.0) == 0.0);
}

TEST_CASE("Test for acos(0.0)") {
	test_assert(acos(0.0) == (acos(-1.0)/2));
}


