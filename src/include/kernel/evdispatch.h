/**
 * @brief  Event dispatcher header
 * @details
 *		Events dispatch without threads
 *
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
 * @date 16.03.2011
 * @author Kirill Tyushev
 */

#ifndef EVDISPATCH_H_
#define EVDISPATCH_H_

#include <kernel/message.h>

/** Structure, describing messages, sent to handlers. */
struct msg {
	/** specified id of message */
	int id;
	/** data in message */
	void *data;
	/** the list of message in message queue */
	struct list_head list;
};

/** maximum count of messages in queue */
#define MAX_MSG_COUNT_IN_QUEUE 10

/**
 * Call handles for all message in queue.
 */
extern void event_dispatch(void);

/**
 * Send message to handler
 * @param id, that corresponds to message
 * @param data, that will be send in message
 */
extern void send_message(int id, void *data);

/**
 * Register handler(function) for message.
 * @param id, that corresponds to message
 * @param handler, that handle message
 */
extern void register_handler(int id, void (*handler)(struct msg *msg));

#endif /* EVDISPATCH_H_ */

