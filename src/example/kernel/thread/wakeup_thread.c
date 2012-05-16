/**
 * @file
 *
 * @brief
 *
 * @date 21.03.2012
 * @author Anton Bondarev
 */
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <kernel/thread/api.h>
#include <kernel/thread/event.h>

#include <framework/example/self.h>

/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

#define CONF_THREADS_QUANTITY      0x8 /* number of executing threads */
#define CONF_HANDLER_REPEAT_NUMBER 10  /* number of circle loop repeats*/
#define TIME_OF_SLEEP              10  /* time second befor threads wake up */

static void *thread_handler(void *args) {
	int i;
	struct event *event = (struct event *)args;

	printf("Sleeping thread %d with id %d\n", (int)thread_self(),
			thread_self()->id);


	sched_sleep(event, SCHED_TIMEOUT_INFINITE);/* sleeping here*/

	/* print a thread structure address and a thread's ID */
	for(i = 0; i < CONF_HANDLER_REPEAT_NUMBER; i ++) {
		printf("Executing thread %d with id %d\n", (int)thread_self(),
				thread_self()->id);
	}
	return thread_self();
}

/**
 * Example's executing routine
 * It has been declared by the macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	struct thread *thr[CONF_THREADS_QUANTITY];
	struct event e;
	void *ret;
	int i;

	event_init(&e, "test_event");

	/* starting all threads */
	for(i = 0; i < ARRAY_SIZE(thr); i ++) {
		printf("starting thread %d with", i);
		thread_create(&thr[i], 0, thread_handler, &e);
		printf(" id = %d\n", thr[i]->id);
	}

	for(i = 0; i < TIME_OF_SLEEP; i++) {
		printf("sleep...\n");
		sleep(1);
	}

	printf("wake up all threads");
	event_fire(&e);

	/* waiting until all threads finish and print return value*/
	for(i = 0; i < ARRAY_SIZE(thr); i ++) {
		thread_join(thr[i], &ret);
		printf("finished thread id %d with result %d\n", i, *((int *)ret));
	}

	return ENOERR;
}
