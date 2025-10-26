//rint_test.c
/**
 * @file
 *
 * @date Oct 13 2025
 * @author Biancaa.R
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("rint() tests");

TEST_CASE("Test for rint() with positive fractional part < 0.5") {
	test_assert(rint(2.3) == 2.0);
}

TEST_CASE("Test for rint() with positive fractional part >= 0.5") {
	test_assert(rint(2.7) == 3.0);
}

TEST_CASE("Test for rint() with negative fractional part > -0.5") {
	test_assert(rint(-2.3) == -2.0);
}

TEST_CASE("Test for rint() with negative fractional part <= -0.5") {
	test_assert(rint(-2.7) == -3.0);
}

TEST_CASE("Test for rint() with exact integer") {
	test_assert(rint(5.0) == 5.0);
}

TEST_CASE("Test for rint() with +INFINITY") {
	test_assert(isinf(rint(INFINITY)));
}

TEST_CASE("Test for rint() with -INFINITY") {
	test_assert(isinf(rint(-INFINITY)));
}

TEST_CASE("Test for rint() with NaN") {
	test_assert(isnan(rint(NAN)));
}

TEST_CASE("Test for rint() with 0.0 and -0.0") {
	test_assert(rint(0.0) == 0.0);
	test_assert(rint(-0.0) == -0.0);
}

 TEST_CASE("Test for rint() with positive fractional part exactly 0.5") {
    test_assert(rint(2.5) == 2.0); // nearest even
    test_assert(rint(3.5) == 4.0); // nearest even
}

TEST_CASE("Test for rint() with negative fractional part exactly -0.5") {
    test_assert(rint(-2.5) == -2.0); // nearest even
    test_assert(rint(-3.5) == -4.0); // nearest even
}
