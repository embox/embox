/**
 * @file
 *
 * @date June 13, 2025
 * @author Jingwei Tang
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


