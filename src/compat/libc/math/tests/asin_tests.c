/**
 * @file
 *
 * @date June 5, 2024
 * @author loharmurtaza
 */

#include <embox/test.h>
#include <math.h>

EMBOX_TEST_SUITE("asin() tests");

TEST_CASE("Test for asin(1.0)") {
  // Correct test for asin(1.0) which should return pi/2
  test_assert(asin(1.0) == M_PI_2);
}

TEST_CASE("Test for asin(0.0)") {
  // This test case is correct
  test_assert(asin(0.0) == 0.0);
}

TEST_CASE("Test for asin(-1.0)") {
  // Correct test for asin(-1.0) which should return -pi/2
  test_assert(asin(-1.0) == -M_PI_2);
}