/**
 * @file
 *
 * @date May 25, 2023
 * @author Anton Bondarev
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("sqrt() tests");

TEST_CASE("Test for sqrt() with negative argument") {
	test_assert(isnan(sqrt(-1.0)));
}

TEST_CASE("Test for sqrt(4.0)") {
	test_assert(sqrt(4.0) == 2.0);
}

TEST_CASE("Test for sqrt(0.0)") {
	test_assert(sqrt(0.0) == 0.0);
}

TEST_CASE("Test for sqrt(+INFINITY)") {
	test_assert(isinf(sqrt(INFINITY)));
}

TEST_CASE("Test for sqrt(NaN)") {
	test_assert(isnan(sqrt(NAN)));
}

TEST_CASE("Test for sqrt(-0.0)") {
	test_assert(sqrt(-0.0) == -0.0);
}
