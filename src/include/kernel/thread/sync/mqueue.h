/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_MQUEUE_H_
#define KERNEL_THREAD_SYNC_MQUEUE_H_

#include <kernel/sched/sched_strategy.h>

#define MQUEUE_SIZE      1024
#define MQUEUE_LEN_SIZE  sizeof(unsigned int)

struct mqueue {
	struct sleepq sleepq;

	unsigned int head;
	unsigned int tail;
	char ring_buffer[MQUEUE_SIZE];
};

/**
 * Initializes given message queue.
 *
 * @param mqueue
 *   Message queue to initialize.
 */
extern void mqueue_init(struct mqueue *mqueue);

/**
 * Adds message to message queue.
 *
 * @param mqueue
 *   Message queue.
 * @param message
 *   Pointer to message.
 * @param len
 *   Length of the message.
 * @retval 0
 *   Adding was successful.
 * @retval non-zero
 *   Adding failed.
 */
extern int mqueue_send(struct mqueue *mqueue, const char *message,
		unsigned int len);

/**
 * Read message from message queue.
 *
 * @param mqueue
 *   Message queue.
 * @param message
 *   Pointer to location where place read message.
 * @retval
 *   Length of the read message.
 */
extern unsigned int mqueue_receive(struct mqueue *mqueue, char *message);

#endif /* KERNEL_THREAD_SYNC_MQUEUE_H_ */
