/**
 * @file
 *
 * @date July 07, 2024
 * @author Ankith Veldandi
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("floor() tests");

TEST_CASE("Test for floor(0.23)") {
	test_assert(floor(0.23) == 0.0);
}

TEST_CASE("Test for floor(5.73)") {
	test_assert(floor(5.73) == 5.0);
}

TEST_CASE("Test for floor(6.0)") {
	test_assert(floor(6.0) == 6.0);
}

TEST_CASE("Test for floor(-3.4)") {
	test_assert(floor(-3.4) == -4.0);
}

TEST_CASE("Test for floor(-3.0)") {
	test_assert(floor(-3.0) == -3.0);
}

TEST_CASE("Test for floor(+INFINITY)") {
	test_assert(isinf(floor(INFINITY)));
}

TEST_CASE("Test for floor(-INFINITY)") {
	test_assert(isinf(floor(-INFINITY)));
}

TEST_CASE("Test for floor(NAN)") {
	test_assert(isnan(floor(NAN)));
}