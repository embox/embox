/**
 * @file
 * @brief IPC messages implementation.
 *
 * @date 25.02.2011
 * @author Dmitry Avdyukhin
 */

#include <kernel/message.h>

#include <stdbool.h>
#include <kernel/scheduler.h>

#define MESSAGE_POOL_SZ 100

/** Pool, containing messages to be occupied. */
static struct message message_pool[MESSAGE_POOL_SZ];
/** Shows what messages are free. */
static bool message_pool_mask[MESSAGE_POOL_SZ];

void message_send(struct message *message, struct thread *thread) {
	scheduler_lock();
	queue_add(&message->list, &thread->messages);
	if (thread->need_message) {
		thread->need_message = false;
		scheduler_wakeup(&thread->msg_event);
	}
	scheduler_unlock();
}

struct message *message_receive(void) {
	if (queue_empty(&current_thread->messages)) {
		current_thread->need_message = true;
		scheduler_sleep(&current_thread->msg_event);
	}
	return (struct message *) list_entry(
		queue_extr(&current_thread->messages), struct message, list);
}

struct message *message_new(void) {
	size_t i;
	for (i = 0; i < MESSAGE_POOL_SZ; i++) {
		if (!message_pool_mask[i]) {
			message_pool_mask[i] = true;
			return message_pool + i;
		}
	}
	return NULL;
}

int message_erase(struct message *message) {
	if (message == NULL) {
		return -1;
	}
	message_pool_mask[message - message_pool] = false;
	return 0;
}
