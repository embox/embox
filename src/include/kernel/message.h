/**
 * @file
 * @brief IPC messages.
 *
 * @date 25.02.2011
 * @author Dmitry Avdyukhin
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <lib/list.h>

struct thread;

typedef int message_type_t;

/**
 * Structure, describing messages, sent to threads.
 */
struct message {
	/** Message type. */
	message_type_t type;
	/** Information in message. */
	void *data;
	/** Node in queue of messages. */
	struct list_head list;
};

/**
 * Send message to thread.
 *
 * @param message sent message
 * @param thread thread to receive message
 */
extern void message_send(struct message *message, struct thread *thread);

/**
 * Allows thread to receive a message.
 *
 * @param thread thread, which wants to take a message.
 * @return received message (wait until there will be such a message).
 */
extern struct message *message_receive(void);

/**
 * Allots memory for new message to send to thread.
 *
 * @return pointer to new message.
 */
extern struct message *message_new(void);

/**
 * Free memory from the message.
 *
 * @param message deleted message.
 * @return -1 if message is NULL
 * @return 0 otherwise.
 */
extern int message_erase(struct message *message);

#endif /* MESSAGE_H_ */
