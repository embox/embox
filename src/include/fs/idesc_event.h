/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_EVENT_H_
#define IDESC_EVENT_H_

#include <kernel/sched/waitq.h>

struct idesc_waitq_link {
	int iwq_masks;
	struct wait_link link;
};

//struct idesc_waitq_link *idesc_wait_link_alloc(void);

extern int idesc_notify_all(struct idesc * idesc, int mask);

/**
 * @brief Prepare link to wait on idesc
 *
 * @param idesc Idesc to wait event on
 * @param wl Idesc_wait_link to prepare
 * @param mask Mask of events
 */
extern int idesc_wait_prepare(struct idesc *idesc, struct idesc_waitq_link *,
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
extern int idesc_wait(struct idesc *idesc, struct idesc_waitq_link *wl,
		unsigned int timeout);

/**
 * @brief Clean idesc_wait_link
 *
 * @param wl
 */
extern void idesc_wait_cleanup(struct idesc *idesc, struct idesc_waitq_link *wl);

#endif /* IDESC_EVENT_H_ */
