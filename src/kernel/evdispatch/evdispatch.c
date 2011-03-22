/**
 * @brief Event dispatcher implementation
 *
 * @date 16.03.2011
 * @author Kirill Tyushev
 */

#include <lib/list.h>
#include <kernel/mm/slab_static.h>
#include <kernel/evdispatch.h>
#include <kernel/messages_defs.h>

/** Handlers, that can handle message with specified id */
struct handler {
	void (*handler)(struct msg *msg);
};

/** pool for message in queue */
STATIC_CACHE_CREATE(queue_msg_cache, struct msg, MAX_MSG_COUNT_IN_QUEUE);

/** Queue of messages, sent to handlers */
LIST_HEAD(queue);

/**
 * Tables of messages(message can be determine by it's id)
 * and functions, that can handle them.
 */
static struct handler handler_arr[MSG_ID_COUNT];

void event_dispatch(void) {
	while (!list_empty(&queue)) {
		struct list_head *result = queue.next;
		struct msg *msg = list_entry(&queue, struct msg, list);
		list_del(result);

		handler_arr[msg->id].handler(msg);

		static_cache_free(&queue_msg_cache, msg);
	}
}

/**
 * Create the message to add it to queue.
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
	list_add_tail(&msg->list, &queue);
}

void register_handler(int id, void (*handler)(struct msg *msg)) {
	handler_arr[id].handler = handler;
}


