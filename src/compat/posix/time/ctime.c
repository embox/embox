/**
 * @file
 * @brief Transform date and time to broken-down time or ASCII
 *
 * @date 21.10.10
 * @author Nikolay Korotky
 */

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

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
			(unsigned short) (time->tm_year + YEAR_1900),
			(unsigned char) (time->tm_mon + 1), (unsigned char) time->tm_mday,
			(unsigned char) time->tm_hour, (unsigned char) time->tm_min,
			(unsigned char) time->tm_sec);
	return buff;
}

char *ctime(const time_t *t) {
	static char __buff[30];
	return ctime_r(t, &__buff[0]);
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
	time_t time = *timep;
	long day_of_month = MONTH_31;

	result->tm_year = EPOCH_START - YEAR_1900;
	result->tm_mon = 0;
	result->tm_mday = 1;
	result->tm_hour = 0;
	result->tm_min = 0;
	result->tm_sec = 0;

	while (time - year_length(result->tm_year + YEAR_1900) >= 0) {
		time -= year_length(result->tm_year + YEAR_1900);
		result->tm_year++;
	}
	while (time - day_of_month >= 0) {
		time -= day_of_month;
		result->tm_mon++;
		day_of_month = days_of_month(result->tm_year, result->tm_mon + 1 /*0..11 -> 1..12*/);
	}
	while (time - DAY_LENGHT >= 0) {
		time -= DAY_LENGHT;
		result->tm_mday++;
	}
	while (time - HOUR_LENGHT >= 0) {
		time -= HOUR_LENGHT;
		result->tm_hour++;
	}
	while (time - MIN_LENGHT >= 0) {
		time -= MIN_LENGHT;
		result->tm_min++;
	}

	result->tm_sec = time;

	return result;
}

struct tm *gmtime(const time_t *timep) {
	static struct tm __tm;
	return gmtime_r(timep, &__tm);
}

time_t mktime(struct tm *tm) {
	time_t time = 0;
	int year, month;

	for (year = EPOCH_START; year < YEAR_1900 + tm->tm_year; year++) {
		time += year_length(year);
	}
	for (month = 1; month <= tm->tm_mon; month++) {
		time += days_of_month(tm->tm_year, month);
	}
	time += DAY_LENGHT * (tm->tm_mday - 1);
	time += HOUR_LENGHT * tm->tm_hour;
	time += MIN_LENGHT * tm->tm_min;
	time += tm->tm_sec;

	return time;
}

static long days_of_month(int year, int month) {
	long days_of_month;

	if (month == 2) {
		days_of_month = (year % 4) ? MONTH_28 : MONTH_29;
	} else if (month < 8) {
		days_of_month = (month % 2) ? MONTH_31 : MONTH_30;
	} else {
		days_of_month = (month % 2) ? MONTH_30 : MONTH_31;
	}

	return days_of_month;
}


char *asctime(const struct tm *timeptr) {
    static char wday_name[7][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
	extern const char months_short_names[12][4];
    static char result[33];

    sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
        wday_name[timeptr->tm_wday],
		months_short_names[timeptr->tm_mon],
        timeptr->tm_mday, timeptr->tm_hour,
        timeptr->tm_min, timeptr->tm_sec,
        1900 + timeptr->tm_year);
    return result;
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
	//TODO local time not use timezone now
	return gmtime_r(timep, result);
}

struct tm *localtime(const time_t *timep) {
	//TODO local time not use timezone now
	return gmtime(timep);
}
