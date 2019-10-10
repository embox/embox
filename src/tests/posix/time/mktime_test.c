/**
 * @file
 *
 * @date May 18, 2019
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <string.h>
#include <time.h>

EMBOX_TEST_SUITE("time/mktime");

TEST_CASE("mktime() ") {
	struct tm time_str;
	time_t t;

	time_str.tm_year = 2001 - 1900;
	time_str.tm_mon = 7 - 1;
	time_str.tm_mday = 4;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 1;
	time_str.tm_isdst = -1;

	t = mktime(&time_str);
	test_assert(t == 994190401);
}
