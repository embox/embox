/**
 * @brief Event Handler implementation
 *
 * @date 16.02.2011
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 */

#include <kernel/message.h>
#include <kernel/thread.h>
#include <kernel/event_handler.h>
#include <kernel/messages_defs.h>

/**
 * Handlers(threads), that can handle message with specified type
 */
struct handlers {
	message_type_t type;
	struct thread *thread;
};

/**
 * Table of messages(message can be determine by it's id) and threads,
 * that can handle them.
 */
static struct handlers msg_array[MESSAGE_COUNT];

/** max id of message */
static int max_id = -1;

/**
 * Create the message to send it to corresponding thread
 *
 * @param id id, that corresponds to type of message
 * @param data data, that will be send in message
 * @return pointer to created message
 */
static struct message *create_message(int id, void *data) {
	struct message *msg = message_new();
	msg->type = msg_array[id].type;
	msg->data = data;
	return msg;
}

void send_message(int id, void *data) {
	struct message *msg = create_message(id, data);
	message_send(msg, msg_array[id].thread);
}

struct message* get_message(void) {
	return message_receive();
}

void register_message(int id, struct thread *thread) {
	max_id++;
	msg_array[max_id].thread = thread;
}
