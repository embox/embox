/**
 * @file
 *
 * @date July 3, 2025
 * @author Thanakrit
 */

#include <math.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fdim() tests");

TEST_CASE("Test for two negative numbers(larger, smaller): fdim(-8, -10)") {
    test_assert(fdim(-8, -10) == 2);
}

TEST_CASE("Test for two negative numbers(smaller, larger): fdim(-10, -8)") {
    test_assert(fdim(-10, -8) == 0.0);
}

TEST_CASE("Test for two positive numbers(larger, smaller): fdim(10, 8)") {
    test_assert(fdim(10, 8) == 2.0);
}

TEST_CASE("Test for two positive numbers(smaller, larger): fdim(8, 10)") {
    test_assert(fdim(8, 10) == 0.0);
}

TEST_CASE("Test for positive first number: fdim(10, -8)") {
    test_assert(fdim(10, -8) == 18);
}

TEST_CASE("Test for negative first number: fdim(-8, 10)") {
    test_assert(fdim(-8, 10) == 0.0);
}

TEST_CASE("fdim(5.0, 3.0) == 2.0") {
    test_assert(fdim(5.0, 3.0) == 2.0);
}

TEST_CASE("fdim(3.0, 5.0) == 0.0") {
    test_assert(fdim(3.0, 5.0) == 0.0);
}

TEST_CASE("fdim(3.0, 3.0) == 0.0") {
    test_assert(fdim(3.0, 3.0) == 0.0);
}

TEST_CASE("fdim(+INFINITY, 42.0) == +INFINITY") {
    test_assert(isinf(fdim(INFINITY, 42.0)));
}

TEST_CASE("fdim(42.0, +INFINITY) == 0.0") {
    test_assert(fdim(42.0, INFINITY) == 0.0);
}

TEST_CASE("fdim(NAN, 1.0) == NaN") {
    test_assert(isnan(fdim(NAN, 1.0)));
}

TEST_CASE("fdim(1.0, NAN) == NaN") {
    test_assert(isnan(fdim(1.0, NAN)));
}
