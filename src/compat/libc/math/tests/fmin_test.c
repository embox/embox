#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fmin() tests");

TEST_CASE("Test case for fmin(0.3, 0.5)") {
    test_assert(fmin(0.3, 0.5) == 0.3);
}

TEST_CASE("Test case for fmin(-3.0, -5.0)") {
    test_assert(fmin(-3.0, -5.0) == -5.0);
}

TEST_CASE("Test case for fmin(+0.0, -0.0)") {
    test_assert(signbit(fmin(+0.0, -0.0)));
}

TEST_CASE("Test case for fmin(INFINITY, 1.0)") {
    test_assert(fmin(INFINITY, 1.0) == 1.0);
}

TEST_CASE("Test case for fmin(-INFINITY, 1.0)") {
    test_assert(fmin(-INFINITY, 1.0) == -INFINITY);
}

TEST_CASE("Test case for fmin(NaN, 5.0)") {
    test_assert(fmin(NAN, 5.0) == 5.0);
}

TEST_CASE("Test case for fmin(5.0, NaN)") {
    test_assert(fmin(5.0, NAN) == 5.0);
}

TEST_CASE("Test case for fmin(NaN, NaN)") {
    test_assert(isnan(fmin(NAN, NAN)));
}
