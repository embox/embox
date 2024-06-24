#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#include <hal/clock.h>
#include <kernel/time/itimer.h>
#include <kernel/time/ktime.h>

#define DEFAULT_N 5000

int main(int argc, char **argv) {
	int N_times = 0;
	struct timespec now;

	if (argc < 2) {
		printf("Usage: ktime_gettv_test <N_times>\n N_times positive "
		       "integer\n");
		return 0;
	}
	else {
		N_times = strtoul(argv[1], NULL, 10);
		if (!N_times) {
			printf("Invalid argument, defaulting to N=%d\n", DEFAULT_N);
			N_times = DEFAULT_N;
		}
	}

	for (int i = 0; i < N_times; i++) {
		ktime_get_timespec(&now);
		printf("now.tv_sec=%ld\tnow.tv_usec=%ld\tnow.tv_nsec=%ld\n", now.tv_sec,
		    now.tv_nsec / 1000, now.tv_nsec);
	}
	return 0;
}
