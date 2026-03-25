#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("fma() tests");

TEST_CASE("Test for fma() with normal arguments") {
    test_assert(fma(2.0, 3.0, 4.0) == 10.0);
    test_assert(fma(-2.0, 3.0, 1.0) == -5.0);
    test_assert(fma(2.0, -3.0, -1.0) == -7.0);
}

TEST_CASE("Test for fma() with zero") {
    test_assert(fma(0.0, 5.0, 2.0) == 2.0);
    test_assert(fma(5.0, 0.0, 2.0) == 2.0);
    test_assert(fma(2.0, 3.0, 0.0) == 6.0);
}

TEST_CASE("Test for fma() with INFINITY") {
    test_assert(isinf(fma(INFINITY, 2.0, 1.0)));
    test_assert(isinf(fma(2.0, INFINITY, 1.0)));
    test_assert(isinf(fma(2.0, 3.0, INFINITY)));
}

TEST_CASE("Test for fma() with NaN") {
    test_assert(isnan(fma(NAN, 2.0, 1.0)));
    test_assert(isnan(fma(2.0, NAN, 1.0)));
    test_assert(isnan(fma(2.0, 3.0, NAN)));
}
