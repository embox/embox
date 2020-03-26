/**
 * @file
 *
 * @date February 28, 2020
 * @author Tamara Deryugina
 */

#include <embox/test.h>

#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include <time.h>

 EMBOX_TEST_SUITE("time/strptime");

 TEST_CASE("checking the correctness of equivalents") {
	 struct tm tm;
	 char *s = " 12/21/44 15:56:60";
	 char *format = "%D %T";

	 char *res = strptime(s, format, &tm);

	 test_assert((tm.tm_sec == 60) && (tm.tm_min == 56) && (tm.tm_hour == 15));
	 test_assert((tm.tm_mday == 21) && (tm.tm_mon == 11) && (tm.tm_year == 144));
	 test_assert(*res == '\0');
 }

 TEST_CASE("checking register of literals before percent") {
	 struct tm tm;
	 char *s = "wArAt 2001-11-15";
	 char *format = "WaRaT %Y-%m-%d";

	 char *res = strptime(s, format, &tm);

	 test_assert(res == NULL);
 }

 TEST_CASE("checking arbitrary whitespaces") {
	 struct tm tm;
	 char *s = "2001 -11-15";
	 char *format = "%Y%t-%m%n-%d";

	 char *res = strptime(s, format, &tm);

	 test_assert((tm.tm_year == 101) && (tm.tm_mon == 10) && (tm.tm_mday == 15));
	 test_assert(res != NULL);
 }

 TEST_CASE("checking string s mathes string format") {
	 struct tm tm;
	 char *s = " 2001-11-25";
	 char *format = "%Y-%m-%d";

	 strptime(s, format, &tm);

	 test_assert((tm.tm_year == 101) && (tm.tm_mon == 10) && (tm.tm_mday == 25));
 }

TEST_CASE("checking different formats") {
	struct tm tm;
	char *s = "mon/22/55/13/35/59";
	char *format = "%a/%d/%y/%H/%M/%S";

	strptime(s, format, &tm);

	test_assert((tm.tm_year == 155) && (tm.tm_wday == 1) && (tm.tm_mday == 22));
	test_assert((tm.tm_hour == 13) && (tm.tm_min == 35) && (tm.tm_sec == 59));
}

TEST_CASE("checking different formats") {
	struct tm tm;
	char *s = " 54:366:dec:6 11";
	char *format = "%C:%j:%b:%w %I";

	strptime(s, format, &tm);

	test_assert((tm.tm_year == 3500) && (tm.tm_wday == 6) && (tm.tm_mon == 11));
	test_assert((tm. tm_hour == 11) && (tm.tm_yday == 365));
}

TEST_CASE("checking returning value in case input string is longer than format string") {
	struct tm tm;
	char *s = "73^5^29^22";
	char *format = "%y^%m^%e";

	char *res = strptime(s, format, &tm);

	test_assert(*res == '^');
}

TEST_CASE("checking returning value in case string s doesn't match string format") {
	struct tm tm;
	char *s = "wed-mar-2020";
	char *format = "%a-%b/%Y";

	char *res = strptime(s, format, &tm);

	test_assert(res == NULL);
}
