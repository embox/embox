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
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sync/message.h>

#define THREAD_STACK_SIZE 0x1000

static char first_stack[THREAD_STACK_SIZE];
static char second_stack[THREAD_STACK_SIZE];
static struct thread *t_first;
static struct thread *t_second;
static struct message *sent_msg;

EMBOX_TEST(run);

/**
 * In the beginning send a message to second, which don't unblock it.
 * Writes "1111...". In the end unblock second thread with proper message.
 */
static void *first_run(void *arg) {
	size_t i;
	struct message *msg = message_new();
	struct message *sec_msg = message_new();
	sent_msg = msg;
	assert(msg);
	sec_msg->type = 3;
	/* Makes nothing, because have wrong type. */
	TRACE("Sending bad message.\n");
	message_send(sec_msg, t_second);
	for (i = 0; i < 1000; i++) {
		TRACE("1");
	}
	msg->type = 2;
	/* Must unblock second thread. */
	TRACE("Sending good message.\n");
	message_send(msg, t_second);
	message_receive();
	TRACE("\nFirst thread received an answer from second one.\n");

	return NULL;
}

/**
 * Waits for a proper message. Then writes "22222...".
 */
static void *second_run(void *arg) {
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
	message_send(msg, t_first);

	return NULL;
}

static int run(void) {
	TRACE("\n");

	t_first = thread_alloc();
	t_second = thread_alloc();

	thread_init(t_first, first_run, first_stack, THREAD_STACK_SIZE);
	thread_init(t_second, second_run, second_stack, THREAD_STACK_SIZE);

	assert(t_first);
	assert(t_second);

	t_first->priority = 1;
	t_second->priority = 2;

	thread_start(t_first);
	thread_start(t_second);

	thread_join(t_first, NULL);
	thread_join(t_second, NULL);

	thread_free(t_first);
	thread_free(t_second);

	return 0;
}
