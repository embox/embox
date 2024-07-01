/**
 * @file
 *
 * @date May 18, 2024
 * @author Jason Mok
 */



#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("ceil() tests");

TEST_CASE("Test for ceil() with positive argument") {
    test_assert(ceil(4.3) == 5.0);
}

TEST_CASE("Test for ceil() with negative argument") {
    test_assert(ceil(-4.3) == -4.0);
}

TEST_CASE("Test for ceil(0.0)") {
    test_assert(ceil(+0.0) == 0.0);
}

TEST_CASE("Test for ceil(-0.0)") {
    test_assert(ceil(-0.0) == 0.0);
}

TEST_CASE("Test for ceil(+INFINITY)") {
    test_assert(isinf(ceil(INFINITY)));
}

TEST_CASE("Test for ceil(NaN)") {
    test_assert(isnan(ceil(NAN)));
}