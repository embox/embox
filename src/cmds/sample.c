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

int *counters;

static void print_usage(void) {
	printf(	"Flags:\n"
			"-h print usage\n"
			"-s start profiling (discard statistics if already running)\n"
			"-l set limit in percents (int)\n"
			"-t stop profiler (do not discard information)\n");
}

static int exec(int argc, char **argv) {
	int i, total = 0, limiter = 0;
	char c;

	counters = get_counters();

	getopt_init();

	while ((c = getopt(argc, argv, "hsl:t")) != -1) {
		switch (c) {
			case 'h':
				print_usage();
				return 0;
				break;
			case 'l':
				sscanf(optarg, "%d", &limiter);
				break;
			case 's':
				printf("Starting profiler...");
				start_profiler();
				printf("Done\n");
				return 0;
				break;
			case 't':
				printf("Stopping profiler...");
				stop_profiler();
				printf("Done\n");
				return 0;
				break;
		}
	}

	if (!sampling_profiler_is_running()) {
		printf("Profiler is not running. Type \"sample -h\" for usage.\n");
		return 0;
	}

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
	return 0;
}
