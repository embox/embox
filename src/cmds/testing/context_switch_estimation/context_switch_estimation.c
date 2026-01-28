/**
 * @file
 *
 * @author Anton Bondarev
 * @date 15.01.26
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


#define HIGH_PRIOR_THEADS_NUM 2
#define LOW_PRIOR_THEADS_NUM  2

#define STATUS_IDLE                      0
#define STATUS_THREAD_READY_TO_WAKEUP    1
#define STATUS_THREAD_DATA_READY         2
#define STATUS_EXIT                      3

struct arguments {
	int id;
	int cnt;
	volatile int status;
	volatile int state;
	pthread_t work_thread;
	struct timespec ts_before;
	struct timespec ts_after;
	struct timespec ts_min;
	struct timespec ts_avg;
	struct timespec ts_max;
	pthread_cond_t cond;
	pthread_mutex_t mut;
};

static void *work_routine(void *arg) {
	struct arguments *a = arg;

	while (a->state != STATUS_EXIT) {
		pthread_mutex_lock(&a->mut);
		a->status = STATUS_THREAD_READY_TO_WAKEUP;
		do {
			pthread_cond_wait(&a->cond, &a->mut);
		
			clock_gettime(CLOCK_MONOTONIC, &a->ts_after);
		} while (a->status == STATUS_THREAD_READY_TO_WAKEUP);

		pthread_mutex_unlock(&a->mut);
	}

	return NULL;
}

int main(int argc, char **argv) {
	struct arguments args[HIGH_PRIOR_THEADS_NUM] = {0};
	int i;
	struct timespec ts_end, ts_cur;
	int minuts;

	if (argc == 1) {
		minuts = 1;
	}
	else {
		minuts = strtoul(argv[1], NULL, 10);
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	ts_end.tv_sec += minuts * 60;

	for (i = 0; i < HIGH_PRIOR_THEADS_NUM; i++) {
		int prio;
		int res;
		int policy;
		struct sched_param par;

		args[i].id = i;
		args[i].status = STATUS_IDLE;
		pthread_mutex_init(&args[i].mut, NULL);
		pthread_cond_init(&args[i].cond, NULL);

		res = pthread_create(&args[i].work_thread, 0, work_routine, &args[i]);
		if (res) {
			printf("couldn't create a thread\n");
			return -EBUSY;
		}

		pthread_getschedparam(pthread_self(), &policy, &par);
		prio = par.sched_priority;
		prio += 10;
		pthread_setschedprio(args[i].work_thread, prio);
	}

	for (i = 0; i < HIGH_PRIOR_THEADS_NUM; i++) {
		args[i].ts_min.tv_nsec = 100000000;
		pthread_detach(args[i].work_thread);
		usleep(100);
	}

	for (int fin = 0; fin == 0;) {

		clock_gettime(CLOCK_MONOTONIC, &ts_cur);
		if (ts_cur.tv_sec > ts_end.tv_sec) {
			fin = 1;
		}

		for (i = 0; i < HIGH_PRIOR_THEADS_NUM; i++) {
			while (args[i].status != STATUS_THREAD_READY_TO_WAKEUP) {};

			pthread_mutex_lock(&args[i].mut);
			if (fin == 1) {
				args[i].state = STATUS_EXIT;
			}
			args[i].state = STATUS_THREAD_DATA_READY;
			pthread_cond_broadcast(&args[i].cond);
			clock_gettime(CLOCK_MONOTONIC, &args[i].ts_before);
			pthread_mutex_unlock(&args[i].mut);
		}
		usleep(100);
		for (i = 0; i < HIGH_PRIOR_THEADS_NUM; i++) {
			struct timespec ts_res;

			timespecsub(&args[i].ts_after, &args[i].ts_before, &ts_res);
			//printf("(%d) duration (%d): %09ld nanosec \n", args[i].cnt, i, (long)args[i].ts_after.tv_nsec);
			//printf("(%d) duration (%d): %09ld nanosec \n", args[i].cnt, i, (long)args[i].ts_before.tv_nsec);
			//printf("(%d) duration (%d): %09ld nanosec \n", args[i].cnt, i, (long)ts_res.tv_nsec);

			args[i].cnt ++;
			if (args[i].ts_min.tv_nsec > ts_res.tv_nsec) {
				args[i].ts_min.tv_nsec = ts_res.tv_nsec;
			}
			if (args[i].ts_max.tv_nsec < ts_res.tv_nsec) {
				args[i].ts_max.tv_nsec = ts_res.tv_nsec;
			}
			args[i].ts_avg.tv_nsec = (args[i].ts_avg.tv_nsec + ts_res.tv_nsec) / 2;
		}
	}
	
	usleep(100);
	for (i = 0; i < HIGH_PRIOR_THEADS_NUM; i++) {
		printf("result usec (%d) (%d): min %09ld avg %09ld max %09ld\n", i, args[i].cnt,
						(long)args[i].ts_min.tv_nsec / 1000,
						(long)args[i].ts_avg.tv_nsec / 1000,
						(long)args[i].ts_max.tv_nsec / 1000);
	}

	printf("Test finished\n");

	return 0;
}
