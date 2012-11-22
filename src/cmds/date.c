/**
 * @file
 * @brief Print or set the system date and time.
 *
 * @date 1.06.2012
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <hal/clock.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: date CCYYMMDDhhmm.ss\n");
}

#define SEC_PER_MINUTE  60
#define SEC_PER_HOUR    3600
#define SEC_PER_DAY     86400
#define SEC_PER_YEAR    31536000 /* 86400 * 365 */
#define SEC_PER_CENTURY 3155760000

static uint32_t sec_per_month[12] = { 2678400, 2419200, 2678400, 2592000,
		2678400, 2592000, 2678400, 2678400, 2592000, 2678400, 2592000, 2678400 };

struct date {
	int CC; /*< century */
	int YY; /*< year */
	int MM; /*< month */
	int DD; /*< day */
	int hh; /*< hour */
	int mm; /*< minute */
	int ss; /*< second */
};

static struct date date;

static void sec_to_date(uint32_t sec) {
	int leap_year_count, i;

	date.CC = 20;
	date.YY = sec / SEC_PER_YEAR; /* years since 1900 */
	leap_year_count = date.YY / 4;
	date.YY -= 100;
	sec = sec % SEC_PER_YEAR - leap_year_count * SEC_PER_DAY;

	/* Add to February one day if current year is a leap year */
	if (date.YY % 4 == 0) {
		sec += SEC_PER_DAY;
		sec_per_month[1] += SEC_PER_DAY;
	}

	for (i = 0; i < 12 && sec > sec_per_month[i]; i++) {
		sec -= sec_per_month[i];
	}
	sec_per_month[1] -= SEC_PER_DAY;

	/* Numeration of days starts with 1 */
	date.MM = i + 1;

	date.DD = sec / SEC_PER_DAY + 1;
	sec %= SEC_PER_DAY;

	date.hh = sec / SEC_PER_HOUR;
	sec %= SEC_PER_HOUR;

	date.mm = sec / SEC_PER_MINUTE;
	sec %= SEC_PER_MINUTE;

	date.ss = sec;
}

static void show_date(void) {
	struct timespec ts;

	getnsofday(&ts, NULL);
	sec_to_date(ts.tv_sec);

	printf("%d.%d.%d%d %d:%d:%d\n", date.DD, date.MM, date.CC, date.YY,
			date.hh, date.mm, date.ss);
}

static void set_date(char *new_date) {
	char *end;

	end = new_date + strlen(new_date);

	/* process seconds */
	end -= 2;
	sscanf(end, "%d", &date.ss);
	end--;
	*end = '\0';

	/* process everything else */
	end -= 2;
	sscanf(end, "%d", &date.mm);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.hh);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.DD);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.MM);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.YY);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.CC);
	*end = '\0';
}

static int exec(int argc, char **argv) {
	int opt;
	struct timeval tv;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hs:"))) {
		printf("\n");
		switch (opt) {
		case 'h': /* show help */
			print_usage();
			break;
		case 's': /* set date*/
			set_date(argv[3]);
			break;
		default:
			break;
		}
	}

	/* show date and kernel time */
	if (argc == 1) {
		show_date();
		ktime_get_timeval(&tv);
		printf("ktime_get_timeval %d:%d (s:ms)\n", (int)tv.tv_sec, (int)tv.tv_usec/1000);
		return 0;
	}

	return 0;
}
