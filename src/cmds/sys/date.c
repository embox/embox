/**
 * @file
 * @brief Print or set the system date and time.
 *
 * @date 1.06.2012
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <hal/clock.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <time.h>

static void print_usage(void) {
	printf("Usage: date -s CCYYMMDDhhmm.ss\n");
}

/* RFC 868 */
#define SECONDS_1900_1970 2208988800L

static void show_date(void) {
	struct timespec ts;
	char buf[256];
	time_t time;

	memset(buf, 0, 256);

	getnsofday(&ts, NULL);
	time = ts.tv_sec;
	ctime_r(&time, buf);

	printf("%s\n", buf);
}

static void set_date(char *new_date) {
	char *end;
	time_t cur_time;
	struct timespec tv;
	struct tm date;

	end = new_date + strlen(new_date);

	/* process seconds */
	end -= 2;
	sscanf(end, "%d", &date.tm_sec);
	end--;
	*end = '\0';

	/* process everything else */
	end -= 2;
	sscanf(end, "%d", &date.tm_min);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.tm_hour);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.tm_mday);
	*end = '\0';

	end -= 2;
	sscanf(end, "%d", &date.tm_mon);
	*end = '\0';

	end -= 4;
	sscanf(end, "%d", &date.tm_year);
	date.tm_year -= 1900;
	*end = '\0';

	cur_time = mktime(&date);
	tv.tv_sec = cur_time;
	tv.tv_nsec = 0;
	setnsofday(&tv, NULL);
}

static int show_fdate(char *fmt) {

	if (fmt[0] != '+') {
		printf("invalid format string, use '+FORMAT'\n");
		return -1;
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);

	char *c = fmt + 1;
	while (*c) {
		if (*c != '%') {
			printf("%c", *c);
			++c;
			continue;
		}

		++c;
		if (!*c) {
			// % ends format, without format specifier.
			// Just print it out as best option;
			printf("%c", '%');
			break;
		}

		switch (*c) {
		case 'N':
			printf("%09llu", (unsigned long long)tv.tv_usec * NSEC_PER_USEC);
			break;
		case 's':
			printf("%lld", (unsigned long long)tv.tv_sec);
			break;
		default:
			printf("%c", *c);
		}

		++c;
	}
	printf("\n");

	return 0;
}

int main(int argc, char **argv) {
	int opt;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hs:"))) {
		printf("\n");
		switch (opt) {
		case 'h': /* show help */
			print_usage();
			break;
		case 's': /* set date*/
			set_date(optarg);
			break;
		default:
			break;
		}
	}

	if (optind < argc) {
		return show_fdate(argv[optind]);
	}

	/* show date and kernel time */
	show_date();
	return 0;
}
