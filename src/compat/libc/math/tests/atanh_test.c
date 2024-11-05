/**
 * @file
 *
 * @date Oct 22, 2024
 * @author LinkolnR
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("atanh() tests");

TEST_CASE("Test for atanh() with negative argument") {
    test_assert(isinf(-atanh(-1.0)));
    printf("Test for atanh(-1.0) passed\n");
}

TEST_CASE("Test for atanh(0.0)") {
    test_assert(atanh(0.0) == 0.0);
    printf("Test for atanh(0.0) passed\n");

}

TEST_CASE("Test for atanh(1.0)") {
    test_assert(isinf(atanh(1.0)));
    printf("Test for atanh(1.0) passed\n");
}

TEST_CASE("Test for atanh(NaN)") {
    test_assert(isnan(atanh(NAN)));
    printf("Test for atanh(NAN) passed\n");

}

