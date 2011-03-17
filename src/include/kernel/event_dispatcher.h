/**
 * @brief  Event Handler header
 * @details it is messenger for receivers(threads) and some senders.
 *
 * @date 16.03.2011
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 */

#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

/**
 * Structure, describing messages, sent to handlers.
 */
struct msg {
	/** specified type of message */
	int id;
	/** data in message */
	void *data;
	/** the list of message in message queue */
	struct list_head next;
};
/**
 * Count of messages. It will be move later in file,
 * witch contains correspondence between messages and it id.
 */
#define MSG_ID_COUNT 10

#include <kernel/message.h>


#define MAX_MSG_COUNT_IN_QUEUE 10

/**
 * Send message to thread
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 */
extern void send_message(int id, void *data);


/**
 * Start dispatcher thread
 */
extern void dispatcher_start(void);

/**
 * Register handler for message.
 */
extern void register_msg(int id, void(*handler)(struct msg *msg));

#endif /* EVENT_HANDLER_H_ */

