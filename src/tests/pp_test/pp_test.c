/**
 * @file
 * @brief test pseudo process
 *
 * @date 16.12.10
 * @author Fedor Burdun
 */

#include <embox/test.h>
#include <kernel/pp.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

EMBOX_TEST(run);

extern char share_variable;

#define THREAD_STACK1_SIZE 0x10000
char stack1[THREAD_STACK1_SIZE];

#define THREAD_STACK2_SIZE 0x10000
char stack2[THREAD_STACK2_SIZE];

void run1() {
	share_variable = 'a';
	while (true) {
		printf("first thread %c\n",share_variable);
		sleep(1);
	}
}

void run2() {
	share_variable = 'b';
	while (true) {
		printf("secont thread %c\n",share_variable);
		sleep(1);
	}
}

static int run(void) {
	int result = 0;
	struct thread *t1, *t2;
	printf("PP TEST: ");

	t1 = thread_create( run1, stack1 + THREAD_STACK1_SIZE );
	t2 = thread_create( run2, stack2 + THREAD_STACK2_SIZE );

	pp_add_process( t1 );
	pp_add_process( t2 );

	thread_start( t1 );
	thread_start( t2 );

	scheduler_start();

	return result;
}

