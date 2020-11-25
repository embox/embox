/**
 * @file
 *
 * @date Nov 25, 2020
 * @author Anton Bondarev
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <drivers/rtc.h>

static void print_usage(void) {
	printf("Usage: rtc [-h] [-a alarm_offset_sec] [-s YEAR:MONTH:DATA:HOUR:MIN:SEC]\n");
	printf("Examples:\n");
	printf("    rtc                        - shows current RTC time\n");
	printf("    rtc -s 2020:11:27:12:10:20 - sets current time to 2020 Nov 27, 12:10:25\n");
	printf("    rtc -a 2                   - set RTC alarm to 2 seconds\n");
}

struct rtc_alarm_data {
	int sec;
	volatile int happened;
	struct rtc_device *rtc;
};

static void rtc_alarm_handler(void *data) {
	struct rtc_alarm_data *alarm_data = data;

	alarm_data->happened = 1;
}

static int rtc_check_alarm(int sec) {
	struct rtc_device *rtc;
	time_t alrm_time;
	struct rtc_wkalrm wkalrm = {0};
	struct tm tm = {0};
	struct rtc_task rtc_task = { 0 };
	struct rtc_alarm_data data = {0};

	printf("RTC alarm will happen in %d sec\n", sec);
	rtc = rtc_get_device(NULL);
	rtc_get_time(rtc, &tm);
	alrm_time = mktime(&tm) + sec + 1;
	gmtime_r(&alrm_time, &(wkalrm.tm));

	rtc_task.func = rtc_alarm_handler;
	rtc_task.private_data = &data;
	rtc_irq_register(rtc, &rtc_task);
	rtc_set_alarm(rtc, &wkalrm);
	while (!data.happened) {
		sleep(1);
	}
	rtc_alarm_irq_enable(rtc, 0);
	rtc_irq_unregister(rtc, &rtc_task);
	printf("RTC alarm has happened\n");
	return 0;
}

int main(int argc, char **argv) {
	int opt;
	long sec;
	struct tm tm;
	struct rtc_device *rtc;

	rtc = rtc_get_device(NULL);

	while (-1 != (opt = getopt(argc, argv, "ha:s:"))) {
		switch (opt) {
		case 's':
			if (!strptime(optarg, "%Y:%m:%d:%H:%M:%S", &tm)) {
				printf("Incorrect date format\n");
				print_usage();

				break;
			}

			rtc_set_time(rtc, &tm);

			break;
		case 'a':
			sec = strtol(optarg, NULL, 0);

			rtc_check_alarm(sec);
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (argc == 1) {
		char buf[128];

		memset(&tm, 0, sizeof tm);

		rtc_get_time(rtc, &tm);

		strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &tm);

		printf("Current RTC time: %s\n", buf);
	}

	return 0;
}
