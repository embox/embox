/**
 * @file
 * @brief Tests messages.
 * @details Test, starting two threads.
 * First thread sends second message of wrong type and doesn't unblock the last one.
 * Then the first one a lot of times writes "1".
 * Then first thread sends message to second and unblocks it.
 * Second thread some times writes "2".
 * First thread can't die until the end of second (waits for a message).
 *
 * @date 30.06.10
 * @author Dmitry Avdyukhin
 */

#include <embox/test.h>
#include <errno.h>
#include <assert.h>
#include <kernel/thread/thread.h>
#include <kernel/thread/sched/sched.h>
#include <kernel/thread/sync/message.h>

#define THREAD_STACK_SIZE 0x1000

static char first_stack[THREAD_STACK_SIZE];
static char second_stack[THREAD_STACK_SIZE];
static struct thread *first_thread;
static struct thread *second_thread;
static struct message *sent_msg;

EMBOX_TEST(run);

/**
 * In the beginning send a message to second, which don't unblock it.
 * Writes "1111...". In the end unblock second thread with proper message.
 */
static void first_run(void) {
	size_t i;
	struct message *msg = message_new();
	struct message *sec_msg = message_new();
	sent_msg = msg;
	assert(msg != NULL);
	sec_msg->type = 3;
	/* Makes nothing, because have wrong type. */
	TRACE("Sending bad message.\n");
	message_send(sec_msg, second_thread);
	for (i = 0; i < 1000; i++) {
		TRACE("1");
	}
	msg->type = 2;
	/* Must unblock second thread. */
	TRACE("Sending good message.\n");
	message_send(msg, second_thread);
	message_receive();
	TRACE("\nFirst thread received an answer from second one.\n");
}

/**
 * Waits for a proper message. Then writes "22222...".
 */
static void second_run(void) {
	size_t i;
	struct message *msg = NULL;
	/* Waits for message with type 2. */
	do {
		if (msg != NULL) {
			message_erase(msg);
		}
		msg = message_receive();
		TRACE("\nMessage type = %d\n", msg->type);
	} while (msg->type != 2);

	message_erase(msg);
	for (i = 0; i < 1000; i++) {
		TRACE("2");
	}
	msg = message_new();
	message_send(msg, first_thread);
}

static int run(void) {
	TRACE("\n");

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
	scheduler_stop();
	return 0;
}
