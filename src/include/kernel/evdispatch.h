/**
 * @brief  Event dispatcher header
 * @details
 *		client    client    client
 *		  |         |         |
 *		  - - - - - - - - - - -
 *		 |     event queue     |
 *        - - - - - - - - - - -
 *                  |
 *                events
 *                  |
 *        - - - - - - - - - - -
 *       |     dispatcher      |
 *        - - - - - - - - - - -
 *        |         |          |
 *     handler1  handler2   handler3
 *
 *     Constans for messages are numbers from 0 to EVENT_MSG_COUNT - 1.
 *     It is necessary to inc EVENT_MSG_COUNT and define constant to add new message.
 *
 * @date 16.03.2011
 * @author Kirill Tyushev
 */

#ifndef EVDISPATCH_H_
#define EVDISPATCH_H_

#include <lib/list.h>
#include <kernel/softirq.h>

/** The count of messages */
#define EVENT_MSG_COUNT 10

/** messages for test */
#define EVENT_MESSAGE1 0
#define EVENT_MESSAGE2 1
#define EVENT_MESSAGE3 2

/** Structure, describing messages, sent to handlers. */
struct event_msg {
	/** specified id of message */
	int id;
	/** data in message */
	void *data;
	/** the list of message in message queue */
	struct list_head list;
};

/**
 * Call handles for all message in queue.
 */
extern void event_dispatch(softirq_nr_t softirq_nr, void *data);

/**
 * Send message to handler
 * @param id, that corresponds to message
 * @param data, that will be send in message
 */
extern void event_send(int id, void *data);

/**
 * Register handler(function) for message.
 * @param id, that corresponds to message
 * @param handler, that handle message
 */
extern void event_register(int id, void (*handler)(struct event_msg *msg));

#endif /* EVDISPATCH_H_ */

