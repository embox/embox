/**
 * @file
 * @brief Tests threads.
 *
 * @date 18.04.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <hal/context.h>
#include <stdio.h>

EMBOX_TEST(run_test);

/**
 * Endlessly writes "+".
 */
static void plus_run(void) {
	while (true) {
		TRACE("+");
	}
}
/**
 * Endlessly writes "-".
 */
static void minus_run(void) {
	while (true) {
		TRACE("-");
	}
}
#define TREAD_STACK_SIZE 0x1000
static char plus_stack[TREAD_STACK_SIZE];
static char minus_stack[TREAD_STACK_SIZE];
static char idle_stack[TREAD_STACK_SIZE];
static struct thread plus_thread;
static struct thread minus_thread;
static struct context bcontext;

#if 0
void threads_init(void) {
	thread_create(&idle_thread, idle_run,
			idle_stack + sizeof(idle_stack));
	TRACE("adasdasd\n");
}
#endif

static int run_test() {
	thread_create(&plus_thread, plus_run, plus_stack + sizeof(plus_stack));
	thread_create(&minus_thread, minus_run, minus_stack + sizeof(minus_stack));
	threads_init();
//	thread_create(&idle_thread, idle_run, idle_stack + sizeof(idle_stack));
	TRACE("before\n");
	idle_thread.run();
	TRACE("after\n");
	idle_run();
#if 0
	context_switch(&bcontext, &idle_thread.thread_context);
#endif
	TRACE("adasdasd\n");
	return 0;
}
