/**
 * @file
 * @brief Event dispatcher implementation
 *
 * @date 28.04.11
 * @author Malkovsky Nikolay
 */

#include <lib/list.h>
#include <mem/misc/pool.h>
#include <kernel/evdispatch2.h>
#include <kernel/thread/sched.h>

/** Maxumum messages count */
#define MAX_MSG_COUNT_IN_QUEUE 10

/** pool for message in queue */
POOL_DEF(msg_queue_pool, struct event_msg, MAX_MSG_COUNT_IN_QUEUE);

/** Queue of messages, sent to handlers */
static LIST_HEAD(msg_queue);

static int CUR_MSG_COUNT_IN_QUEUE = 0;

#include <embox/unit.h>
#define make_cache_name(name) \
		""#name""
#include <mem/misc/slab.h>
EMBOX_UNIT_INIT(event_dispatcher_init);

static int event_dispatcher_init(void) {
	cache_t *cache;

	cache = cache_create(make_cache_name(msg_queue_cache), sizeof(struct event_msg), MAX_MSG_COUNT_IN_QUEUE);

	return 0;
}

void event_dispatch() {
	while (!list_empty(&msg_queue)) {
		struct list_head *result = msg_queue.next;
		struct event_msg *msg = list_entry(result, struct event_msg, list);
		list_del(result);

        msg->handler(msg->data);

		pool_free(&msg_queue_pool, msg);
	}
}

/**
 * Create the message to add it to queue.
 *
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 * @return pointer to created message
 */
static struct event_msg *create_message(void (*handler)(void *data), void *data) {
	struct event_msg *msg = pool_alloc(&msg_queue_pool);
	msg->handler = handler;
	msg->data = data;
	return msg;
}

/**
 * Sends message.
 *
 * @param handler, handler to handle a massage
 * @param data, message data
 */
void event_send(void (*handler)(void *data), void *data) {
	struct event_msg *msg;

	msg = create_message(handler, data);
	sched_lock();
	if(CUR_MSG_COUNT_IN_QUEUE == MAX_MSG_COUNT_IN_QUEUE) {
		// TODO For more information here we may return error code.
		return;
	}
	++CUR_MSG_COUNT_IN_QUEUE;
	list_add_tail(&msg->list, &msg_queue);
	sched_unlock();
}
