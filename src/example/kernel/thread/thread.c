/**
 * @file
 *
 * @brief Create 'n' parallel executed thread
 *
 * @details This example demonstrates API for creation several threads and
 *      their parallel execution. First of all here create THREADS_QUANTITY
 *      thread this parameter you can define during example configure.
 *      As a thread's handler every thread pass the same function
 *      'thread_handler'. There is a loop in which print address of thread
 *      structure and thread ID. Number of repeats you can specify with
 *      macro HANDLER_REPEAT_NUMBER.
 *      Main program waiting when all thread finish with calling thread_joing
 *      and print return code of each thread.
 *
 * @date 16.08.2011
 * @author Anton Bondarev
 */
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <util/array.h>

#include <kernel/thread.h>

#include <framework/example/self.h>


/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

/* configs */
#define CONF_THREADS_QUANTITY      0x8 /* number of executing threads */
#define CONF_HANDLER_REPEAT_NUMBER 300  /* number of circle loop repeats*/

/** The thread handler function. It's used for each started thread */
static void *thread_handler(void *args) {
	int i;
	/* print a thread structure address and a thread's ID */
	for(i = 0; i < CONF_HANDLER_REPEAT_NUMBER; i ++) {
		printf("%d", *(int *)args);
	}
	return thread_self();
}

/**
 * Example's executing routine
 * It has been declared by the macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	struct thread *thr[CONF_THREADS_QUANTITY];
	int data[CONF_THREADS_QUANTITY];
	void *ret;
	int i;

	/* starting all threads */
	for(i = 0; i < ARRAY_SIZE(thr); i ++) {
		data[i] = i;
		//printf("starting thread %d with", i);
		thr[i] = thread_create(0, thread_handler, &data[i]);
		//printf(" id = %d\n", thr[i]->id);
	}

	/* waiting until all threads finish and print return value*/
	for(i = 0; i < ARRAY_SIZE(thr); i ++) {
		thread_join(thr[i], &ret);
		//printf("finished thread id %d with result %d\n", i, *((int *)ret));
	}
	printf("\n");

	return ENOERR;
}
