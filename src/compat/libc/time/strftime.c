/**
 * @file
 * @brief Convert date and time to a formatted string
 *
 * @date 20.10.12
 * @author Nikolay Korotkiy
 */

#include <assert.h>
#include <time.h>
#include <stdio.h>

#define YEAR_BASE       1900

/**
 * %C - The century number (year/100) as a 2-digit integer
 * %Y - The year as a decimal number including the century
 * %y - The year as a decimal number without a century (range 00 to 99)
 * %m - The month as a decimal number (range 01 to 12)
 * %d - The day of the month as a decimal number zero-padded (range 01 to 31)
 * %e - The day of the month as a decimal number space-padded (range  1 to 31)
 * %H - The hour as a decimal number using a 24-hour clock (range 00 to 23)
 * %M - The minute as a decimal number (range 00 to 59)
 * %S - The second as a decimal number (range 00 to 60)
 * %T - The time in 24-hour notation (%H:%M:%S)
 */
size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
    static char mon_name[12][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
	size_t count = 0;

	assert(s != NULL);
	assert(fmt != NULL);
	assert(tm != NULL);

	for (;;) {
		while (*fmt && *fmt != '%') {
			if (count < max - 1) {
				s[count++] = *fmt++;
			} else {
				return 0;
			}
		}
		if (*fmt == '\0') {
			break;
		}
		fmt++;
		switch (*fmt) {
		case 'C':
			sprintf(&s[count], "%2d", (tm->tm_year + YEAR_BASE) / 100);
			count += 2;
			break;
		case 'Y':
			sprintf(&s[count], "%4d", tm->tm_year + YEAR_BASE);
			count += 4;
			break;
		case 'y':
			sprintf(&s[count], "%2d", tm->tm_year % 100);
			count += 2;
			break;
		case 'm':
			sprintf(&s[count], "%2d", tm->tm_mon + 1);
			count += 2;
			break;
		case 'b':
			sprintf(&s[count], "%3s", mon_name[tm->tm_mon]);
			count += 3;
			break;
		case 'd':
			sprintf(&s[count], "%02d", tm->tm_mday);
			count += 2;
			break;
		case 'e':
			sprintf(&s[count], "%2d", tm->tm_mday);
			count += 2;
			break;
		case 'H':
			sprintf(&s[count], "%02d", tm->tm_hour);
			count += 2;
			break;
		case 'M':
			sprintf(&s[count], "%02d", tm->tm_min);
			count += 2;
			break;
		case 'S':
			sprintf(&s[count], "%02d", tm->tm_sec);
			count += 2;
			break;
		case 'T':
			sprintf(&s[count], "%02d:%02d:%02d",
				tm->tm_hour, tm->tm_min, tm->tm_sec);
			count += 8;
			break;
		default:
			return 0;
		}

		if (*fmt) {
			fmt++;
		} else {
			break;
		}
	}

	if (max) {
		s[count] = '\0';
	}

	return count;
}
