/**
 * @brief Event dispatcher implementation
 *
 * @date 16.03.2011
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 */

#include <lib/list.h>
#include <kernel/thread.h>
#include <kernel/mm/slab_static.h>
#include <kernel/event_dispatcher.h>
#include <kernel/messages_defs.h>

#define THREAD_STACK_SIZE 0x10000

/** Stack for dispatcher_thread. */
static char dispatcher_stack[THREAD_STACK_SIZE];

/**
 * Handlers, that can handle message with specified type
 */
struct handler_thread {
	int msg_type;
	struct thread *handler;
};

struct handler_function {
	void (*handler)(struct msg *msg);
};

STATIC_CACHE_CREATE(queue_msg_cache, struct msg, MAX_MSG_COUNT_IN_QUEUE);

/** Queue of messages, sent to handlers */
static struct list_head *queue;
/**
 * Two tables of messages(message can be determine by it's id) and threads
 * and functions, that can handle them.
 */
static struct handler_thread handler_thread_arr[MSG_ID_COUNT];
static struct handler_function handler_func_arr[MSG_ID_COUNT];

static void event_dispatch(void) {
	while (1) {
		if (!list_empty(queue)) {
			struct list_head *result = queue->next;
			struct msg *msg = list_entry(queue, struct msg, next);
			list_del(result);
			/* if it is not thread, then it is function */
			if (msg->id < MIN_THREAD_ID) {
				handler_func_arr[msg->id].handler(msg);
			} else {
				struct message *message = message_new();
				message->data = msg->data;
				message_send(message, handler_thread_arr[msg->id].handler);
			}
			static_cache_free(&queue_msg_cache, msg);
		}
	}
}

void dispatcher_start(void) {
	struct thread *dispatcher = thread_create(event_dispatch, dispatcher_stack
			+ THREAD_STACK_SIZE);
	dispatcher->priority = 0;
	thread_start(dispatcher);
}

/**
 * Create the message to send it to corresponding thread
 *
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 * @return pointer to created message
 */
static struct msg *create_message(int id, void *data) {
	struct msg *msg = static_cache_alloc(&queue_msg_cache);
	msg->id = id;
	msg->data = data;
	return msg;
}

void send_message(int id, void *data) {
	struct msg *msg;
	msg = create_message(id, data);
	list_add(&msg->next, queue);
}

void register_msg_for_func(int id, void (*handler)(struct msg *msg)) {
	handler_func_arr[id].handler = handler;
}

void register_msg_for_thread(int id, struct thread *thread) {
	handler_thread_arr[id].handler = thread;
}
