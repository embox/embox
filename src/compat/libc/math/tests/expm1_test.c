/**
 * @file
 *
 * @date Nov 10, 2024
 * @author Jason Mok
 */

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("expm1() tests");

TEST_CASE("Test for expm1(0.0)") {
    test_assert(expm1(0.0) == 0.0);
}

TEST_CASE("Test for expm1(3.0)") {
    test_assert(expm1(3.0) == (exp(3.0) - 1.0));
}

TEST_CASE("Test for expm1(1.0)") {
    test_assert(expm1(1.0) == (M_E - 1.0));
}

TEST_CASE("Test for expm1(-1.0)") {
    test_assert(expm1(-1.0) == (-1.0 / M_E));
}

TEST_CASE("Test for expm1(+INFINITY)") {
    test_assert(isinf(expm1(INFINITY)));
}

TEST_CASE("Test for expm1(-INFINITY)") {
    test_assert(expm1(-INFINITY) == -1.0);
}

TEST_CASE("Test for expm1(NaN)") {
    test_assert(isnan(expm1(NAN)));
}
