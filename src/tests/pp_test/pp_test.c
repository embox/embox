/**
 * @file
 * @brief test pseudo process
 *
 * @date 16.12.10
 * @author Fedor Burdun
 */

#include <embox/test.h>
#include <kernel/pp.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>

EMBOX_TEST(run);

extern char *share_variable;

#define THREAD_STACK_SIZE 0x10000

static char stack1[THREAD_STACK_SIZE];
static char stack2[THREAD_STACK_SIZE];

void *run1(void *arg) {
	share_variable = "string1";
	while (true) {
		printf("first thread %s\n",share_variable);
		//sleep(1);
	}
}

void *run2(void *arg) {
	share_variable = "string2";
	while (true) {
		printf("secont thread %c\n", share_variable);
		sleep(1);
	}
}

static int run(void) {
	struct thread t1, t2;

	thread_init(&t1, run1, stack1, THREAD_STACK1_SIZE);
	thread_init(&t2, run2, stack2, THREAD_STACK2_SIZE);

	pp_add_process(&t1);
	pp_add_process(&t2);

	thread_start(&t1);
	thread_start(&t2);

	thread_join(&t1, NULL);
	thread_join(&t2, NULL);

	return 0;
}

