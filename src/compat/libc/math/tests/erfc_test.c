/**
 * @file
 *
 * @date September 10, 2025
 * @author Stanislav Kidun
 */

#include <math.h>
#include <float.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("erfc() tests");

TEST_CASE("Test for  erfc(0) == 1 - erf(0)") {
    double arg = 0;
    test_assert(erfc(arg) == 1 - erf(arg));
}

TEST_CASE("Test for erfc(+INFINITY)") {
	test_assert(erfc(INFINITY) == 0.0);
}

TEST_CASE("Test for erfc(-INFINITY)") {
	test_assert(erfc(-INFINITY) == 2.0);
}

TEST_CASE("Test for erfc(NaN)") {
	test_assert(isnan(erfc(NAN)));
}