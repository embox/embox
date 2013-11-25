/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_EVENT_H_
#define IDESC_EVENT_H_

#include <kernel/sched/wait_queue.h>

#define IDESC_EVENT_READ         0x0001
#define IDESC_EVENT_WRITE        0x0002
#define IDESC_EVENT_ERROR        0x0004

struct idesc_event {
	int flags;
	struct wait_link wait_link;
};

extern int idesc_event_lock(void);

extern int idesc_event_unlock(void);

extern int idesc_notify_all(struct idesc * idesc, int mask);

/* for SCHED_TIMEOUT_INFINITE */
#include <kernel/sched.h>

/**
 * @brief Wait for events of specifed mask occured on idesc
 *
 * @param idesc Idesc to wait event on
 * @param mask Mask of events
 * @param timeout Timeout of waiting specified in ms (?). Can be SCHED_TIMEOUT_INFINITE.
 *
 * @return -EAGAIN if descriptor has O_NONBLOCK set
 * @return -ETIMEDOUT if timeout expired during waiting
 * @return -EINTR if was interrupted
 * @return Non-negative if event occured
 */
extern int idesc_wait(struct idesc *idesc, int mask, unsigned int timeout);

#endif /* IDESC_EVENT_H_ */
