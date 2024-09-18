/**
 * @file
 *
 * @author Anton Bondarev
 * @date 13.06.24
 */

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

struct arguments {
	int id;
	pthread_t work_thread;
};

static int cnt = 0;

static inline void delay(time_t delay_ms) {
	struct timespec ts_before, ts, ts_res;
	time_t ms;

	clock_gettime(CLOCK_MONOTONIC, &ts_before);

	for (;;) {
		clock_gettime(CLOCK_MONOTONIC, &ts);
		timespecsub(&ts, &ts_before, &ts_res);
		ms = ts_res.tv_sec * 1000 + ts_res.tv_nsec / 1000000L;
		if (ms > delay_ms) {
			//printf("delay(%ld)\n", ms);
			break;
		}
	}
}

static void *work_routine(void *arg) {
	struct arguments *a = arg;
	struct timespec ts_before, ts_after, ts_res;
	int ms_delay;

	clock_gettime(CLOCK_MONOTONIC, &ts_before);
	printf("Start thread id(%d) time: %ld.%03ld sec\n", a->id,
	    (long)ts_before.tv_sec, (long)ts_before.tv_nsec / 1000000L);

	switch(a->id) {
	case 0:
		ms_delay = 200;
		break;

	case 1:
		ms_delay = 300;
		break;

	case 2:
		ms_delay = 500;
		break;

	default:
		cnt++;
		return NULL;
	}

	delay(ms_delay);

	clock_gettime(CLOCK_MONOTONIC, &ts_after);
	timespecsub(&ts_after, &ts_before, &ts_res);
	printf("End thread id(%d) time: %ld.%03ld sec ", a->id,
	    (long)ts_after.tv_sec, (long)ts_after.tv_nsec / 1000000L);
	printf("duration: %ld.%03ld sec \n", (long)ts_res.tv_sec,
	    (long)ts_res.tv_nsec / 1000000L);

	cnt++;

	return NULL;
}

int main(void) {
	struct arguments args[3];
	int prio;
	int res;
	int policy;
	struct sched_param par;
	int i;


	for (i = 0; i < 3; i++) {
		args[i].id = i;
		res = pthread_create(&args[i].work_thread, 0, work_routine, &args[i]);
		if (res) {
			printf("couldn't create a thread\n");
			return -EBUSY;
		}

		pthread_getschedparam(pthread_self(), &policy, &par);
		prio = par.sched_priority;
		prio += (i);
		prio -= 3;
		pthread_setschedprio(args[i].work_thread, prio);
	}
	cnt = 0;
	for (i = 0; i < 3; i++) {
		pthread_detach(args[i].work_thread);
		usleep(10);
	}

	while (cnt < 3) {
		sleep(1);
	}

	printf("Test finished\n");

	return 0;
}
