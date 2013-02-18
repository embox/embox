/**
 * @file
 * @brief
 *
 * @date 18.02.13
 * @author Alexander Lapshin
 */

#include <embox/test.h>
#include <stdlib.h>


EMBOX_TEST_SUITE("standard library");

TEST_CASE("Check strtod function") {
    test_assert_equal(0,(int)atof("0.0"));
    test_assert_equal(1, (int) atof("1.0"));
    test_assert_equal(-1,(int)atof("-1.0"));
    test_assert_equal(1,(int)atof("1.1"));
    test_assert_equal(0, (int) atof("0.11"));
    test_assert_equal(-1,(int)atof("-1.1"));
	}
