/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_EVENT_H_
#define IDESC_EVENT_H_

#include <kernel/sched/waitq.h>

#define IDESC_EVENT_READ         0x0001
#define IDESC_EVENT_WRITE        0x0002
#define IDESC_EVENT_ERROR        0x0004

struct idesc_event {
	int flags;
	struct wait_link wait_link;
};

struct idesc_wait_link {

};

extern int idesc_event_lock(void);

extern int idesc_event_unlock(void);

extern int idesc_notify_all(struct idesc * idesc, int mask);

/**
 * @brief Prepare link to wait on idesc
 *
 * @param idesc Idesc to wait event on
 * @param wl Idesc_wait_link to prepare
 * @param mask Mask of events
 */
extern void idesc_wait_prepare(struct idesc *idesc, struct idesc_wait_link *wl,
		int mask);

/* for SCHED_TIMEOUT_INFINITE */
#include <kernel/sched.h>

/**
 * @brief Wait for events of specifed mask occured on idesc
 *
 * @param wl Prepared idesc_wait_link
 * @param timeout Timeout of waiting specified in ms (?). Can be SCHED_TIMEOUT_INFINITE.
 *
 * @return -EAGAIN if descriptor has O_NONBLOCK set
 * @return -ETIMEDOUT if timeout expired during waiting
 * @return -EINTR if was interrupted
 * @return Non-negative if event occured
 */
extern int idesc_wait(struct idesc_wait_link *wl, unsigned int timeout);

/**
 * @brief Clean idesc_wait_link
 *
 * @param wl
 */
extern void idesc_wait_cleanup(struct idesc_wait_link *wl);

#endif /* IDESC_EVENT_H_ */
