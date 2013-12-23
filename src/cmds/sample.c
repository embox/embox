/**
 * @file
 * @brief TODO check if profiler is running
 *
 * @date 15.12.2013
 * @author Denis Deryugin
 */

#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <embox/cmd.h>
#include <util/array.h>
#include <profiler/sampling/sample.h>

EMBOX_CMD(exec);

typedef enum {START_PROFILING, STOP_PROFILING, SHOW_INFO} action;

int *counters;

static void print_usage(void) {
	printf(	"Flags:\n"
			"-h print usage\n"
			"-s start profiling (discard statistics if already running)\n"
			"-l set limit in percents (int)\n"
			"-t stop profiler (do not discard information)\n"
			"-i set custom timer interval\n");
}

static int exec(int argc, char **argv) {
	int i, total = 0, limiter = 0, interval = 100;
	char c;
	action act = SHOW_INFO;

	counters = get_counters();

	getopt_init();

	while ((c = getopt(argc, argv, "hsl:ti:")) != -1) {
		switch (c) {
			case 'i':
				if (1 != sscanf(optarg, "%d", &interval)) {
					printf("Wrong argument, integer value for \"-i\" expected.\n");
					return 0;
				}
				break;
			case 'h':
				print_usage();
				return 0;
				break;
			case 'l':
				if (1 != sscanf(optarg, "%d", &limiter)) {
					printf("Wrong argument, integer value for \"-l\" expected.\n");
					return 0;
				}
				break;
			case 's':
				act = START_PROFILING;
				break;
			case 't':
				act = STOP_PROFILING;
				break;
		}
	}

	switch(act) {
		case START_PROFILING:
			if (!sampling_profiler_is_running()){
				printf("Starting profiler...\n");
			} else {
				printf("Restarting profiler...\n");
			}
			start_profiler(interval);
			break;
		case STOP_PROFILING:
			if (sampling_profiler_is_running()) {
				printf("Stopping profiler...\n");
				stop_profiler();
				return 0;
			}
		case SHOW_INFO:
			/*if (!sampling_profiler_is_running()) {
				printf("Profiler is not running. Type \"sample -h\" for usage.\n");
				return 0;
			}*/

			printf("Sampling information:\n");
			printf("%5s %10s   %s\n", "%", "Counter", "Function");
			for (i = 0; i < SAMPLE_HASH_SIZE; i++) {
				total += counters[i];
			}

			for (i = 0; i < SAMPLE_HASH_SIZE; i++) {
				if ((double)100 * counters[i] / total > limiter) {
					printf("%5.2lf %10d   %s\n", (double) 100.0 * counters[i] / total,
						counters[i], get_name(i));
				}
			}
	}

	return 0;
}
