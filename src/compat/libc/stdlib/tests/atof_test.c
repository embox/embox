/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Alexander Lapshin
 */

#include <embox/test.h>
#include <stdlib.h>

EMBOX_TEST_SUITE("standard library");

TEST_CASE("Check atof() function") {
    test_assert_equal(0,   (int) atof("0.0"));
    test_assert_equal(10,  (int) (10 * atof("1.0")));
    test_assert_equal(-10, (int) (10 * atof("-1.0")));
    test_assert_equal(11,  (int) (10 * atof("1.1")));
    test_assert_equal(11,  (int) (100 * atof("0.11")));
    test_assert_equal(-11, (int) (10 * atof("-1.1")));
    test_assert_equal(-110,(int) atof("-1.1E+2"));
    test_assert_equal(-1,  (int) (100 * atof("-0.1e-1")));
    test_assert_equal(10,  (int) atof("0.1E2"));
}
