/**
 * @brief  Event Handler header
 * @details it is messenger for receivers(threads) and some senders.
 *
 * @date 16.03.2011
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <kernel/message.h>

/**
 * Send message to thread
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 */
extern void send_message(int id, void *data);

/**
 * Receive a message.
 * @return received message (wait until there will be such a message).
 */
extern struct message* get_message(void);

/**
 * register handler for message
 * @param id, that corresponds to type of message
 * @param thread, that will be process message
 */
extern void register_message(int id, struct thread *thread);

#endif /* EVENT_HANDLER_H*/
