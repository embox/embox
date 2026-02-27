/**
 * @file
 *
 * @date Feb 24, 2026
 * @author Dmitry Pilyuk
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "sys/time.h"

int main(int argc, char **argv) {
	struct timespec now, prev;
	int i = 0;
	
	clock_gettime(CLOCK_MONOTONIC, &prev);
	clock_gettime(CLOCK_MONOTONIC, &now);

	while (timespeccmp(&now, &prev, >=)) {
		prev = now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		// ktime_get_timespec(&now);
		i++;
	}
	printf("Wrong time\n");
	printf("i = %d\n", i);
	printf("Now = %ld sec %ld nsec\n", now.tv_sec, now.tv_nsec);
	printf("Prev = %ld sec %ld nsec\n", prev.tv_sec, prev.tv_nsec);
	return 0;
}
