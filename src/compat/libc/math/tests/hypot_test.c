/**
 * @file
 * 
 * @date December 2, 2025
 * @author Vladislav Novikov
 */

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("hypot() tests");

TEST_CASE("Test hypot(3, 4) = 5") {
    test_assert(hypot(3, 4) == 5.0);
}

TEST_CASE("Test hypot(0, 0) = 0") {
    test_assert(hypot(0, 0) == 0.0);
}

TEST_CASE("Test hypot(5, 0) = 5") {
    test_assert(hypot(5, 0) == 5.0);
}

TEST_CASE("Test hypot(0, 5) = 5") {
    test_assert(hypot(0, 5) == 5.0);
}

TEST_CASE("Test hypot with negative arguments") {
    test_assert(hypot(-3, 4) == 5.0);
    test_assert(hypot(3, -4) == 5.0);
    test_assert(hypot(-3, -4) == 5.0);
}

TEST_CASE("Test hypot with INFINITY") {
    test_assert(hypot(INFINITY, 1) == INFINITY);
    test_assert(hypot(1, INFINITY) == INFINITY);
    test_assert(isinf(hypot(INFINITY, INFINITY)));
}

TEST_CASE("Test hypot with NAN") {
    test_assert(isnan(hypot(NAN, 1)));
    test_assert(isnan(hypot(1, NAN)));
    test_assert(isnan(hypot(NAN, NAN)));
}

TEST_CASE("Test hypot with -NAN") {
    test_assert(isnan(hypot(-NAN, 1)));
    test_assert(isnan(hypot(1, -NAN)));
    test_assert(isnan(hypot(-NAN, -NAN)));
}

TEST_CASE("Test hypot with large numbers") {
    double x = 1e308;
    double y = 1e308;
    double expected = 1.4142135623730951e308;

    test_assert(fabs(hypot(x, y) - expected) < 1e292);
}

TEST_CASE("Test hypot with small numbers") {
    double x = 1e-308;
    double y = 1e-308;
    double expected = 1.4142135623730951e-308;

    test_assert(fabs(hypot(x, y) - expected) < 1e-322);
}

TEST_CASE("Test hypot symmetry property") {
    double x = 3.5;
    double y = 7.2;

    test_assert(hypot(x, y) == hypot(y, x));
}