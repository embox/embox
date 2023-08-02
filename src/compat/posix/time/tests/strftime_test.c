/**
 * @file
 *
 * @date May 18, 2019
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <string.h>
#include <time.h>

EMBOX_TEST_SUITE("time/strftime");

TEST_CASE("strftime ") {
	struct tm time_str;
	char daybuf[20];

	time_str.tm_year = 2001 - 1900;
	time_str.tm_mon = 7 - 1;
	time_str.tm_mday = 4;
	time_str.tm_hour = 0;
	time_str.tm_min = 0;
	time_str.tm_sec = 1;
	time_str.tm_wday = 3;
	time_str.tm_yday = 184;
	time_str.tm_isdst = 1;

	(void)strftime(daybuf, sizeof(daybuf), "%A", &time_str);

	test_assert(0 == strncmp(daybuf, "Wednesday", sizeof(daybuf)));
}
