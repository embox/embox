/**
 * @file
 * @brief Transform date and time to broken-down time or ASCII
 *
 * @date 21.10.10
 * @author Nikolay Korotky
 */

#include <time.h>
#include <stdio.h>

#define EPOCH_START 1900
#define YEAR_LENGHT 31556926
#define MONTH_28    2419200
#define MONTH_29    2505600
#define MONTH_30    2592000
#define MONTH_31    2678400
#define DAY_LENGHT  86400
#define HOUR_LENGHT 3600
#define MIN_LENGHT  60

/* From  specification: might be overwritten by subsequent calls to any of the date and time functions */
static struct tm __tm;

static long days_of_month(struct tm *tm);

char *ctime_r(const time_t *t, char *buff) {
	struct tm *time = gmtime(t);

	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
			time->tm_year, time->tm_mon, time->tm_mday, time->tm_hour,
			time->tm_min, time->tm_sec);
	return buff;
}

struct tm *gmtime(const time_t *timep) {
	time_t time = *timep;
	long day_of_month = MONTH_31;

	__tm.tm_year = EPOCH_START;
	__tm.tm_mon = 1;
	__tm.tm_mday = 1;
	__tm.tm_hour = 0;
	__tm.tm_min = 0;
	__tm.tm_sec = 0;

	while (time - YEAR_LENGHT >= 0) {
		time -= YEAR_LENGHT;
		__tm.tm_year++;
	}
	while (time - day_of_month >= 0) {
		time -= day_of_month;
		__tm.tm_mon++;
		day_of_month = days_of_month(&__tm);
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
	time_t time;

	time = YEAR_LENGHT * tm->tm_year;
	time += days_of_month(tm) * tm->tm_mon;
	time += DAY_LENGHT * (tm->tm_mday - 1);
	time += HOUR_LENGHT * tm->tm_hour;
	time += MIN_LENGHT * tm->tm_min;
	time += tm->tm_sec;

	return time;
}

static long days_of_month(struct tm *tm) {
	long day_of_month;

	if (tm->tm_mon == 2) {
		day_of_month = (tm->tm_year % 4) ? MONTH_29 : MONTH_28;
	} else if (tm->tm_mon < 8) {
		day_of_month = (tm->tm_mon % 2) ? MONTH_31 : MONTH_30;
	} else {
		day_of_month = (tm->tm_mon % 2) ? MONTH_30 : MONTH_31;
	}

	return day_of_month;
}
