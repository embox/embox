/**
 * @file
 * @brief Tests messages.
 *
 * @date 30.06.2010
 * @author Dmitry Avdyukhin
 */
#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <assert.h>

#define THREAD_STACK_SIZE 0x1000

static char first_stack[THREAD_STACK_SIZE];
static char second_stack[THREAD_STACK_SIZE];
static struct thread *first_thread;
static struct thread *second_thread;

static struct message *sent_msg;

EMBOX_TEST(run_test)
;


/**
 * In the beginning send a message to second, which don't unblock it.
 * Writes "1111...". In the end unblock second thread with proper message.
 */
static void first_run(void) {
	int i;
	struct message *msg = msg_new();
	struct message *sec_msg = msg_new();
	sent_msg = msg;
	assert(msg != NULL);
	msg->type = 1;
	sec_msg->type = 3;
	/* Makes nothing, because have wrong type. */
	msg_send(msg, second_thread);
	msg_send(sec_msg, second_thread);
	for (i = 0; i < 1000; i++) {
		TRACE("1");
	}
	msg->type = 2;
	/* Must unblock second thread. */
	msg_send(msg, second_thread);
}

/**
 * Waits for a proper message. Then writes "22222...".
 */
static void second_run(void) {
	struct message *msg = NULL;
	/* Waits for message with type 2. */
	do {
		if (msg != NULL) {
			msg_erase(msg);
		}
		msg = msg_receive(second_thread);
		TRACE("\nMessage type = %d\n", msg->type);
	} while (msg->type != 2);

	msg_erase(msg);
	for (int i = 0; i < 1000; i++) {
		TRACE("2");
	}
}

/**
 * Test, starting two threads.
 * First thread sends second message of wrong type and doesn't unblock the last one.
 * Then the first one a lot of times writes "1". Then first thread sends message to
 *   second and unblocks it.
 * Second thread some times writes "2".
 *
 * @retval 0 if test is passed
 * @retval -EINVAL if an error occurs.
 */
static int run_test() {
	TRACE("\n");
	scheduler_init();

	first_thread = thread_create(first_run, first_stack + THREAD_STACK_SIZE);
	second_thread = thread_create(second_run, second_stack + THREAD_STACK_SIZE);

	assert(first_thread != NULL);
	assert(second_thread != NULL);

	first_thread->priority = 1;
	second_thread->priority = 2;

	thread_start(first_thread);
	thread_start(second_thread);

	TRACE("\nBefore start\n");
	scheduler_start();

	return 0;
}
