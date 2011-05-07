/**
 * @brief  Event dispatcher header
 * @details
 	This implementation of event dispatching doesn't bind message to
 	a specific number, and then by number returning the handler.
 	Here we will bind message to a handler directly.
 *
 * @date 28.04.11
 * @author Malkovsky Nikolay
 */

#ifndef EVDISPATCH2_H_
#define EVDISPATCH2_H_

#include <lib/list.h>

/** Structure, describing messages, sent to handlers. */
struct event_msg {
	/** specified handler to handle the message */
	void (*handler)(void *data);
	/** data in message */
	void *data;
	/** the list of message in message queue */
	struct list_head list;
};

/**
 * Call handles for all message in queue.
 */
extern void event_dispatch(void);

/**
 * Send message to handler
 * @param id, that corresponds to message
 * @param data, that will be send in message
 */
extern void event_send(void (*handler)(void *data), void *data);


#endif /* EVDISPATCH_H_ */
