/**
 * @brief Event dispatcher implementation
 *
 * @date 16.03.2011
 * @author Kirill Tyushev
 */

#include <lib/list.h>
#include <util/pool.h>
#include <kernel/softirq.h>
#include <kernel/evdispatch.h>

/** maximum count of messages in queue */
#define MAX_MSG_COUNT_IN_QUEUE 10

/** Handlers, that can handle message with specified id */
struct handler {
	void (*handler)(struct event_msg *msg);
};

/** pool for message in queue */
POOL_DEF(struct event_msg, msg_queue_cache, MAX_MSG_COUNT_IN_QUEUE);

/** Queue of messages, sent to handlers */
static LIST_HEAD(msg_queue);

/**
 * Tables of messages(message can be determine by it's id)
 * and functions, that can handle them.
 */
static struct handler handler_arr[EVENT_MSG_COUNT];

void event_dispatch(softirq_nr_t softirq_nr, void *data) {
	while (!list_empty(&msg_queue)) {
		struct list_head *result = msg_queue.next;
		struct event_msg *msg = list_entry(result, struct event_msg, list);
		list_del(result);

		handler_arr[msg->id].handler(msg);

		static_cache_free(&msg_queue_cache, msg);
	}
}

/**
 * Create the message to add it to queue.
 *
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 * @return pointer to created message
 */
static struct event_msg *create_message(int id, void *data) {
	struct event_msg *msg = static_cache_alloc(&msg_queue_cache);
	msg->id = id;
	msg->data = data;
	return msg;
}

void event_send(int id, void *data) {
	struct event_msg *msg;

	if (id > EVENT_MSG_COUNT - 1)
			return;

	msg = create_message(id, data);
	list_add_tail(&msg->list, &msg_queue);
}

void event_register(int id, void (*handler)(struct event_msg *msg)) {
	if (id > EVENT_MSG_COUNT - 1)
		return;

	handler_arr[id].handler = handler;
}
