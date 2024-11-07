/**
 * @file
 *
 * @date Nov 6, 2024
 * @author Lidia Domingos
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("exp2() tests");

TEST_CASE("Test for exp2(3.0)") {
    test_assert(exp2(3.0) == 8.0);
}

TEST_CASE("Test for exp2(0.0)") {
    test_assert(exp2(0.0) == 1.0);
}

TEST_CASE("Test for exp2(-1.0)") {
    test_assert(exp2(-1.0) == 0.5);
}

TEST_CASE("Test for exp2(+INFINITY)") {
    test_assert(isinf(exp2(INFINITY)));
}

TEST_CASE("Test for exp2(NaN)") {
    test_assert(isnan(exp2(NAN)));
}

TEST_CASE("Test for exp2(-INFINITY)") {
    test_assert(exp2(-INFINITY) == 0.0);
}
