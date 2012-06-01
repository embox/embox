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

#include <kernel/clock_source.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	/* TODO usecase printf("Usage: date [[[[[CC]YY]MM]DD]hh]mm[.ss]\n"); */
	printf("Usage: date CCYYMMDDhhmm.ss\n");
}

#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MINUTE 60

struct date {
	int CC; /*< century */
	int YY; /*< year */
	int MM; /*< month */
	int DD; /*< day */
	int hh; /*< hour */
	int mm; /*< minute */
	int ss; /*< second */
};

/* Time in seconds since last show_date() or set_date() call. */
static clock_t last_sec = 0;

/* There are one and only one such structure in system.
 * Access to it throw show_date/set_date functions. */
static struct date date;

/* TODO Now I suppose that time refresh at least once at day.
 * Date bound to timer */
static void sec_to_date(int sec) {
	short ss, mm, hh, DD;
	int tmp;

	tmp = sec;

	DD = tmp / SEC_PER_DAY;
	tmp -= DD * SEC_PER_DAY;

	hh = tmp / SEC_PER_HOUR;
	tmp -= hh * SEC_PER_HOUR;

	mm = tmp / SEC_PER_MINUTE;
	tmp -=  mm * SEC_PER_MINUTE;

	ss = tmp;

	date.DD += DD;
	date.hh += hh;
	date.mm += mm;
	date.ss += ss;

	/* check for overflows */
	if (date.ss >= 60) {
		date.ss -= 60;
		date.mm++;
	}

	if (date.mm >= 60) {
		date.mm -= 60;
		date.hh++;
	}

	if (date.hh >= 24) {
		date.hh -= 24;
		date.DD++;
	}

	/* TODO YY and CC. YY may be leap-year. */
}

static void show_date(void) {
	clock_t delta;

	delta = clock_sys_sec() - last_sec;
	last_sec += delta;
	sec_to_date(delta);

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

	/* show date */
	if (argc == 1) {
		show_date();
		return 0;
	}

	return 0;
}
