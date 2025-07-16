/**
 * @file
 *
 * @date April 05, 2025
 * @author Atharv Dubey
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("lrint() tests");

TEST_CASE("Test for lrint(0.23)") {
	test_assert(lrint(0.23) == 0L);
}

TEST_CASE("Test for lrint(0.73)") {
	test_assert(lrint(0.73) == 1L);
}

TEST_CASE("Test for lrint(0.0)") {
	test_assert(lrint(0.0) == 0L);
}

TEST_CASE("Test for lrint(3.5)") {
	test_assert(lrint(3.5) == 4L);
}

TEST_CASE("Test for lrint(-3.5)") {
	test_assert(lrint(-3.5) == -4L);
}
