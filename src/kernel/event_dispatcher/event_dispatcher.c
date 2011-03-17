/**
 * @brief Event Handler implementation
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

struct handler {
	void (*handler)(struct msg *msg);

struct handlers {
	message_type_t type;
	struct thread *thread;

};

STATIC_CACHE_CREATE(queue_msg_cache, struct msg, MAX_MSG_COUNT_IN_QUEUE);

static struct list_head *queue;
/**
 * Table of messages(message can be determine by it's id) and threads,
 * that can handle them.
 */
static struct handler handlers_array[MSG_ID_COUNT];


static void event_dispatch(void) {
	while (1) {
		if (!list_empty(queue)) {
			struct list_head *result = queue->next;
			struct msg *msg = list_entry(queue, struct msg, next);
			list_del(result);
			handlers_array[msg->id].handler(msg);
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

/** max id of message */
static int max_id = -1;


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


void register_msg(int id, void (*handler)(struct msg *msg)){
     handlers_array[id].handler = handler;
}
