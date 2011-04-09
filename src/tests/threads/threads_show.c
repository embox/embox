/**
 * @file
 * @brief Tests function thread_yield.
 * @details Test, which a lot of times writes "+", "-", "*" and "!".
 * Because of priorities they are printed in such order:
 * 1) "*";
 * 2) "!";
 * 3) "+" and "-".
 * For each symbol there is a thread, which shows it.
 * These threads show one symbol and switch context.
 *
 * @date 09.05.10
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char plus_stack[THREAD_STACK_SIZE];
static char minus_stack[THREAD_STACK_SIZE];
static char mult_stack[THREAD_STACK_SIZE];
static char highest_stack[THREAD_STACK_SIZE];
static struct thread *t_plus;
static struct thread *t_minus;
static struct thread *t_mult;
static struct thread *t_highest;

EMBOX_TEST(run)
;

/**
 * Endlessly Writes "!". Thread with the highest priority.
 */
static void *highest_run(void *arg) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("!");
		thread_yield();
	}

	return NULL;
}

/**
 * Endlessly Writes "+".
 */
static void *plus_run(void *arg) {
	size_t i;
	for (i = 0; i < 100; i++) {
		TRACE("+");
		thread_yield();
	}

	return NULL;
}

/**
 * Endlessly writes "-".
 */
static void *minus_run(void *arg) {
	size_t i;
	for (i = 0; i < 110; i++) {
		TRACE("-");
		thread_yield();
	}

	return NULL;
}

/**
 * Endlessly writes "*".
 */
static void *mult_run(void *arg) {
	size_t i;
	for (i = 0; i < 120; i++) {
		TRACE("*");
		thread_yield();
	}

	return NULL;
}

static int run(void) {
	TRACE("\n");

	t_plus = thread_alloc();
	thread_init(t_plus, plus_run, plus_stack, THREAD_STACK_SIZE);
	assert(t_plus);
	t_plus->priority = 3;

	t_minus = thread_alloc();
	thread_init(t_minus, minus_run, minus_stack, THREAD_STACK_SIZE);
	assert(t_minus);
	t_minus->priority = 3;

	t_mult = thread_alloc();
	thread_init(t_mult, mult_run, mult_stack, THREAD_STACK_SIZE);
	assert(t_mult);

	t_highest = thread_alloc();
	thread_init(t_highest, highest_run, highest_stack, THREAD_STACK_SIZE);
	assert(t_highest);
	t_highest->priority = 2;

	thread_start(t_plus);
	thread_start(t_minus);
	thread_start(t_mult);
	thread_start(t_highest);

	thread_join(t_plus);
	thread_join(t_minus);
	thread_join(t_mult);
	thread_join(t_highest);

	thread_free(t_plus);
	thread_free(t_minus);
	thread_free(t_mult);
	thread_free(t_highest);

	return 0;
}
