/**
 * @brief  Event Handler header
 * @details it is messenger for receivers(threads and functions) and some senders.
 *
 * @date 16.03.2011
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 */

#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

#include <kernel/message.h>

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

/** id of threads are bigger than functions id.  */
#define MIN_THREAD_ID 10

/** maximum count of messages in queue */
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
 * Register handler(function) for message.
 */
extern void register_msg_for_func(int id, void(*handler)(struct msg *msg));

/**
 * Register handler(function) for message.
 */
extern void register_msg_for_thread(int id, struct thread *thread);

#endif /* EVENT_HANDLER_H_ */

