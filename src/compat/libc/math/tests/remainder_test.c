/**
 * @file
 *
 * @date May 11, 2025
 * @author Shankari Anand
 */

#include <math.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("remainder() tests");

TEST_CASE("Test for remainder() with positive numerator") {
    double result = remainder(5.0, 2.0);
    test_assert(result == 1.0);
}

TEST_CASE("Test for remainder() with negative numerator") {
    double result = remainder(-5.0, 2.0);
    test_assert(result == -1.0);
}

TEST_CASE("Test for remainder() with negative denominator") {
    double result = remainder(5.0, -2.0);
    test_assert(result == 1.0);
}

TEST_CASE("Test for remainder() when both are negative") {
    double result = remainder(-5.0, -2.0);
    test_assert(result == -1.0);
}

TEST_CASE("Test for remainder() with zero numerator") {
    double result = remainder(0.0, 2.0);
    test_assert(result == 0.0);
}
