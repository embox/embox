/**
 * @file
 * @brief TODO documentation for core_test.c -- Alina Kramar
 *
 * @date 09.04.2011
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>

EMBOX_TEST_SUITE("test for thread API");

#define THREAD_STACK_SIZE 0x1000

static char minus_stack[THREAD_STACK_SIZE];

static void *minus_run(void *arg) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("-");
	}

	return NULL;
}

TEST_CASE("") {
	struct thread minus_thread;

	minus_thread = thread_init(minus_run, minus_stack, THREAD_STACK_SIZE);

	thread_join(&minus_thread, NULL);
}
