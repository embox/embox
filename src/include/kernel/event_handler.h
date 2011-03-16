/**
 * @brief  Event Handler header
 * @details it is messenger for receivers(threads) and some senders.
 *
 * @date 16.03.2011
 * @author Alexandr Kalmuk
 */

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#define MESSAGE_COUNT 10

/**
 * Send message to thread
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 */
extern void send_message(int id, void *data);

#endif /* EVENT_HANDLER_H*/
