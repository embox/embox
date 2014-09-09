/**
 * @file
 * @brief Print or set the system date and time.
 *
 * @date 1.06.2012
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <hal/clock.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <time.h>

EMBOX_CMD(exec);

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
	settimeofday(&tv, NULL);
}

static char *get_next_key(char *string) {
	while(*string++) {
		if('%' == *string) {
			return ++string;
		}
	}
	return NULL;
}
#define MAX_NANOSECONDS 999999999
static int check_format(char *string) {
	char *str;
	if(string[0] != '+') {
		printf("invalid format string use '+%%N'\n");
		return -1;
	}
	if(NULL != (str = get_next_key(string))) {
		switch(str[0]) {
		case 'N': {
			struct timeval tv;
			time64_t ns;

			ktime_get_timeval(&tv);
			ns = timeval_to_ns(&tv);
			printf("%lld\n", ns % MAX_NANOSECONDS);
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

static int exec(int argc, char **argv) {
	int opt;
	//struct timeval tv;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hs:"))) {
		printf("\n");
		switch (opt) {
		case 'h': /* show help */
			print_usage();
			break;
		case 's': /* set date*/
			set_date(argv[2]);
			break;
		default:
			break;
		}
	}

	/* show date and kernel time */
	if (argc == 1) {
		show_date();
		//ktime_get_timeval(&tv);
		//printf("ktime_get_timeval %d:%d (s:ms)\n", (int)tv.tv_sec, (int)tv.tv_usec/1000);
		return 0;
	}
	check_format(argv[argc-1]);

	return 0;
}
