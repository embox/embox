/**
 * @file
 *
 * @date October 11, 2025
 * @author Kirill Khitsko
*/

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("tan() tests");

TEST_CASE("Test for tan(+INFINITY)") {
	test_assert(isnan(tan(INFINITY)));
}

TEST_CASE("Test for tan(0.0)") {
	test_assert(tan(0.0) == 0.0);
}

TEST_CASE("Test for tan(-pi/4) negative argument") {
	test_assert(fabs(tan(-M_PI_4) + 1.0) < 1e-10);
}

TEST_CASE("Test for tan(pi/2)") {
	test_assert(isinf(tan(M_PI_2)) || tan(M_PI_2) > 1e10);
}

TEST_CASE("Test for tan(pi)") {
	test_assert(tan(M_PI) < 1e-10);
}

TEST_CASE("Test for tan(NaN)") {
	test_assert(isnan(tan(NAN)));
}