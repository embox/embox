/**
 * @file
 *
 * @date 21.10.10
 * @author Nikolay Korotky
 */
#include <time.h>
#include <stdio.h>

#define EPOCH_START 1970
#define YEAR_LENGHT 31556926
#define MONTH_28    2419200
#define MONTH_29    2505600
#define MONTH_30    2592000
#define MONTH_31    2678400
#define DAY_LENGHT  86400
#define HOUR_LENGHT 3600
#define MIN_LENGHT  60

char *ctime(const time_t *t, char *buff) {
	time_t time = *t;
	unsigned int year = EPOCH_START, month = 1, day = 1, hour = 0, min = 0;
	long day_of_month = MONTH_31;
	while (time - YEAR_LENGHT >= 0) {
		time -= YEAR_LENGHT;
		year++;
	}
	while (time - day_of_month >= 0) {
		time -= day_of_month;
		month++;
		if (month == 2) {
			day_of_month = (year % 4) ? MONTH_29 : MONTH_28;
		} else if (month < 8) {
			day_of_month = (month % 2) ? MONTH_31 : MONTH_30;
		} else {
			day_of_month = (month % 2) ? MONTH_30 : MONTH_31;
		}
	}
	while (time - DAY_LENGHT >= 0) {
		time -= DAY_LENGHT;
		day++;
	}
	while (time - HOUR_LENGHT >= 0) {
		time -= HOUR_LENGHT;
		hour++;
	}
	while (time - MIN_LENGHT >= 0) {
		time -= MIN_LENGHT;
		min++;
	}
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, (int)time);
	return buff;
}
