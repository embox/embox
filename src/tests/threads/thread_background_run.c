/**
 * @file  thread_background_run.c
 * @brief thread test for show memory statistic
 *
 * @date  06.12.10
 * @author Gleb Efimov
 * @author Alina Kramar
 * @author Roman Oderov
 */

#include <embox/test.h>
#include <kernel/thread.h>

#define THREAD_STACK_SIZE 0x1000

static char highest_stack[THREAD_STACK_SIZE];

EMBOX_TEST(run);

static void highest_run(void) {
	for (int i = 0; i < 100000; i++) {
		thread_yield();
	}
}

static int run(void) {
	struct thread *t = thread_create(highest_run, highest_stack + THREAD_STACK_SIZE );
	thread_start(t);
	return 0;
}
