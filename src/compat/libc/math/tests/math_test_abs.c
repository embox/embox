/**
 * @file
 *
 * @date May 18, 2024
 * @aothor Guokai Chen
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("abs() tests");

#define abs fabs

TEST_CASE("Test for abs() with negative argument") {
	test_assert(abs(-1.0) == 1.0);
}

TEST_CASE("Test for abs() with positive argument") {
	test_assert(abs(4.0) == 4.0);
}

TEST_CASE("Test for abs(+0.0)") {
	test_assert(abs(+0.0) == 0.0);
}

TEST_CASE("Test for abs(-0.0)") {
	test_assert(abs(-0.0) == 0.0);
}

TEST_CASE("Test for abs(+INFINITY)") {
	test_assert(isinf(abs(INFINITY)));
}

TEST_CASE("Test for abs(NaN)") {
	test_assert(isnan(abs(NAN)));
}