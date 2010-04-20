/**
 * @file
 * @brief Tests threads.
 *
 * @date 18.04.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>

EMBOX_TEST(run);

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
#define TREAD_STACK_SIZE 0x400
static char plus_stack[TREAD_STACK_SIZE];
static char minus_stack[TREAD_STACK_SIZE];
static struct thread plus_thread;
static struct thread minus_thread;

static int run() {
	thread_create(&plus_thread, plus_run, plus_stack + sizeof(plus_stack));
	thread_create(&minus_thread, minus_run, minus_stack + sizeof(minus_stack));
	threads_init();
	return 0;
}
