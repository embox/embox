/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_EVENT_H_
#define IDESC_EVENT_H_

#include <kernel/sched/waitq.h>
#include <poll.h> /* for flags */

struct idesc;

/**
 * The same as struct wait_link link but have poll flags
 */
struct idesc_wait_link {
	int iwq_masks;
	struct waitq_link link;
};

static inline void idesc_wait_init(struct idesc_wait_link *iwl, int mask) {
	iwl->iwq_masks = mask;
	waitq_link_init(&iwl->link);
}

/**
 * @brief Prepare link to wait on idesc, cheking O_NONBLOCK of descriptor.
 *
 * @param idesc Idesc to wait event on
 * @param wl Idesc_wait_link to prepare
 * @param mask Mask of events
 *
 * @return 0 on success
 * @return -EAGAIN if descriptor has O_NONBLOCK set
 */
extern int idesc_wait_prepare(struct idesc *idesc, struct idesc_wait_link *wl);

/**
 * @brief Prepare link to wait on idesc.
 * @param i
 * @param wl
 * @param mask
 */
extern int idesc_wait_do_prepare(struct idesc *i, struct idesc_wait_link *wl);
/**
 * @brief Wait for events of specified mask occurred on idesc
 *
 * @param wl Prepared idesc_wait_link
 * @param timeout Timeout of waiting specified in ms (?). Can be SCHED_TIMEOUT_INFINITE.
 *
 * @return -ETIMEDOUT if timeout expired during waiting
 * @return -EINTR if was interrupted
 * @return Non-negative if event occured
 */
extern int idesc_wait(struct idesc *idesc, unsigned int timeout);

/**
 * @brief Clean idesc_wait_link
 *
 * @param idesc - index descriptor on which thread waited
 * @param wl wait link which was prepared by idesc_wait_prepare()
 */
extern void idesc_wait_cleanup(struct idesc *idesc, struct idesc_wait_link *wl);

/**
 * Wake all thread which wait this descriptor
 *
 * @param idesc on which something happened
 */
extern int idesc_notify(struct idesc *idesc, int mask);

#define IDESC_WAIT_LOCKED(_unlock_expr, _idesc, _iwl, _mask, _timeout, _lock_expr) \
	({                                                   \
	 	int __res = 0;                                   \
		idesc_wait_init(_iwl, _mask);                    \
                                                         \
		__res = idesc_wait_prepare(_idesc, _iwl);        \
		if (!__res) {                                    \
			_unlock_expr;                                \
			__res = idesc_wait(_idesc, _timeout); \
			_lock_expr;                                  \
                                                         \
			idesc_wait_cleanup(_idesc, _iwl);            \
		}                                                \
	 	__res;                                           \
	})


#endif /* IDESC_EVENT_H_ */
