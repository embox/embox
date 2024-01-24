/**
 * @file
 * @brief TODO check if profiler is running
 *
 * @date 15.12.2013
 * @author Denis Deryugin
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <unistd.h>
#include <lib/libds/array.h>
#include <profiler/sampling/sample.h>

typedef enum {START_PROFILING, STOP_PROFILING, SHOW_INFO} action;

struct entry {
	int number, counter;
};

int entry_cmp(const void *fst, const void *snd){
	return - ((struct entry *)fst)->counter + ((struct entry *)snd)->counter;
}

int *counters;
struct entry entries[SAMPLE_HASH_SIZE];

static void print_usage(void) {
	printf(	"Flags:\n"
			"-h print usage\n"
			"-s start profiling (discard statistics if already running)\n"
			"-l show top N entries\n"
			"-t stop profiler (do not discard information)\n"
			"-i set custom timer interval\n");
}

int main(int argc, char **argv) {
	int i, total_counter = 0, total_entries = 0, limiter = 0, interval = 100;
	char c;
	action act = SHOW_INFO;

	counters = get_counters();


	while ((c = getopt(argc, argv, "hsl:ti:")) != (char) -1) {
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
			} else {
				printf("Profiler is not running!\n");
			}
			return 0;
		case SHOW_INFO:
			/*if (!sampling_profiler_is_running()) {
				printf("Profiler is not running. Type \"sample -h\" for usage.\n");
				return 0;
			}*/

			printf("Sampling information:\n");
			printf("%5s %10s   %s\n", "  ", "Counter", "Function");
			for (i = 0; i < SAMPLE_HASH_SIZE; i++) {
				total_counter += counters[i];
				if (counters[i] != 0)
					total_entries++;
				entries[i].counter = counters[i];
				entries[i].number = i;
			}

			qsort(entries, SAMPLE_HASH_SIZE, sizeof(struct entry), entry_cmp);

			if (limiter == 0)
				limiter = total_entries;

			for (i = 0; i < limiter; i++) {
				printf("%5.2lf%% %9d   %s\n", (double) 100.0 * entries[i].counter / total_counter,
					entries[i].counter, get_name(entries[i].number));
			}
	}

	return 0;
}
