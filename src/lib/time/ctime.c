/**
 * @file
 * @brief Transform date and time to broken-down time or ASCII
 *
 * @date 21.10.10
 * @author Nikolay Korotky
 */

#include <time.h>
#include <stdio.h>

#define YEAR_1900 1900
#define EPOCH_START 1970
#define MONTH_28    2419200
#define MONTH_29    2505600
#define MONTH_30    2592000
#define MONTH_31    2678400
#define DAY_LENGHT  86400
#define HOUR_LENGHT 3600
#define MIN_LENGHT  60

static long days_of_month(int year, int month);
static bool is_leap_year(int year);

#define year_length(year) \
		(is_leap_year(year) ? 31622400 : 31536000)

/* From  specification: might be overwritten by subsequent calls to any of the date and time functions */
static struct tm __tm;

static bool is_leap_year(int year) {
	if (year % 400 == 0) {
		return true;
	} else if (year % 100 == 0) {
	   return false;
	} else if (year % 4 == 0) {
	   return true;
	}

	return false;
}

char *ctime_r(const time_t *t, char *buff) {
	struct tm *time = gmtime(t);

	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
			time->tm_year + YEAR_1900, time->tm_mon + 1, time->tm_mday, time->tm_hour,
			time->tm_min, time->tm_sec);
	return buff;
}

struct tm *gmtime(const time_t *timep) {
	time_t time = *timep;
	long day_of_month = MONTH_31;

	__tm.tm_year = EPOCH_START - YEAR_1900;
	__tm.tm_mon = 0;
	__tm.tm_mday = 1;
	__tm.tm_hour = 0;
	__tm.tm_min = 0;
	__tm.tm_sec = 0;

	while (time - year_length(__tm.tm_year + YEAR_1900) >= 0) {
		time -= year_length(__tm.tm_year + YEAR_1900);
		__tm.tm_year++;
	}
	while (time - day_of_month >= 0) {
		time -= day_of_month;
		__tm.tm_mon++;
		day_of_month = days_of_month(__tm.tm_year, __tm.tm_mon + 1 /*0..11 -> 1..12*/);
	}
	while (time - DAY_LENGHT >= 0) {
		time -= DAY_LENGHT;
		__tm.tm_mday++;
	}
	while (time - HOUR_LENGHT >= 0) {
		time -= HOUR_LENGHT;
		__tm.tm_hour++;
	}
	while (time - MIN_LENGHT >= 0) {
		time -= MIN_LENGHT;
		__tm.tm_min++;
	}

	__tm.tm_sec = time;

	return &__tm;
}

time_t mktime(struct tm *tm) {
	struct tm tmp;
	time_t time;

	tmp = *tm;

	while (--tmp.tm_year >= EPOCH_START) {
		time += year_length(tmp.tm_year + YEAR_1900);
	}
	while (--tmp.tm_mon > 0) {
		time += days_of_month(tmp.tm_year, tmp.tm_mon + 1);
	}
	time += DAY_LENGHT * (tm->tm_mday - 1);
	time += HOUR_LENGHT * tm->tm_hour;
	time += MIN_LENGHT * tm->tm_min;
	time += tm->tm_sec;

	return time;
}

static long days_of_month(int year, int month) {
	long day_of_month;

	if (month == 2) {
		day_of_month = (year % 4) ? MONTH_29 : MONTH_28;
	} else if (month < 8) {
		day_of_month = (month % 2) ? MONTH_31 : MONTH_30;
	} else {
		day_of_month = (month % 2) ? MONTH_30 : MONTH_31;
	}

	return day_of_month;
}
