/**
 * @file
 *
 * @date Aug 1, 2023
 * @author Stepan Klyukin
 */

#include <embox/test.h>

#include <string.h>
#include <time.h>

EMBOX_TEST_SUITE("time/gmtime");

TEST_CASE("gmtime()") {
	struct tm time_str;
	time_t t = 1630436560;

    time_str = *gmtime(&t);

    test_assert_equal(time_str.tm_year, 2021 - 1900);
    test_assert_equal(time_str.tm_mon, 8 - 1);
    test_assert_equal(time_str.tm_mday, 31);
    test_assert_equal(time_str.tm_hour, 19);
    test_assert_equal(time_str.tm_min, 2);
    test_assert_equal(time_str.tm_sec, 40);
}
