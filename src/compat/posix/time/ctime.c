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
#define DAY_LENGHT  86400
#define HOUR_LENGHT 3600
#define MIN_LENGHT  60

#define DAYS_IN_1970 719499
#define JD_OF_EPOCH 2440588    /* Julian Date of noon, J1970 */

#define year_length(year) \
		(is_leap_year(year) ? 31622400 : 31536000)

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

static void days_to_date(time_t jd, int *year, int *month, int *day)
{
	long l, n, i, j, d, m, y;

	l = jd + 68569;
	n = (4 * l) / 146097;
	l = l - (146097 * n + 3) / 4;
	i = (4000 * (l + 1)) / 1461001;
	l = l - (1461 * i) / 4 + 31;
	j = (80 * l) / 2447;
	d = l - (2447 * j) / 80;
	l = j / 11;
	m = j + 2 - 12 * l;
	y = 100 * (n - 49) + i + l;

	*year  = y;
	*month = m;
	*day   = d;
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
	time_t epoch, jdn;
	int year, month, day, hour, min, sec;

	epoch = *timep; /* Get the seconds since the EPOCH */

	jdn = epoch / DAY_LENGHT;
	epoch -= DAY_LENGHT * jdn;

	hour = epoch / HOUR_LENGHT;
	epoch -= HOUR_LENGHT * hour;

	min = epoch / MIN_LENGHT;
	epoch -= MIN_LENGHT * min;

	sec = epoch;

	days_to_date(jdn + JD_OF_EPOCH, &year, &month, &day);

	result->tm_year   = year - YEAR_1900; /* Relative to 1900 */
	result->tm_mon    = month - 1;           /* zero-based */
	result->tm_mday   = day;                 /* one-based */
	result->tm_hour   = hour;
	result->tm_min    = min;
	result->tm_sec    = sec;

	return result;
}

struct tm *gmtime(const time_t *timep) {
	static struct tm __tm;
	return gmtime_r(timep, &__tm);
}

time_t mktime(struct tm *tm) {
	int mon = tm->tm_mon + 1;
	int year = tm->tm_year + 1900;
	int days, hours;

	mon -= 2;
	if (0 >= (int)mon) {	/* 1..12 -> 11, 12, 1..10 */
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	days = (year / 4 - year / 100 + year / 400 +
			367 * mon / 12 + tm->tm_mday) +
			year * 365 - DAYS_IN_1970;
	hours = days * 24 + tm->tm_hour;
	return (hours * 60 + tm->tm_min) * 60 + tm->tm_sec;
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
