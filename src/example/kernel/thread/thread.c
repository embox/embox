/**
 * @file
 *
 * @brief
 *
 * @date 16.08.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <framework/example/self.h>
#include <kernel/thread/api.h>

/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

#define THREADS_QUANTITY      0x8 /* number of executing threads */

static void *thread_handler(void *args) {
	int i;
	for(i = 0; i < 10; i ++) {
		printf("Executing thread %d\n", (int)thread_self());
	}
	return thread_self();
}

/**
 * Example's executing routine
 * It has been declared by the macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	struct thread *thr[THREADS_QUANTITY];
	void *ret;
	int i;

	printf("Start thread's example\n");
	/* starting all threads */
	for(i = 0; i < THREADS_QUANTITY; i ++) {
		printf("starting thread id %d\n", i);
		thread_create(&thr[i], 0, thread_handler, NULL);
	}

	/* waiting until all threads finish and print return value*/
	for(i = 0; i < THREADS_QUANTITY; i ++) {
		thread_join(thr[i], &ret);
		printf("finished thread id %d with result %d\n", i, *((int *)ret));
	}
	printf("Finish thread's example\n");

	return ENOERR;
}
