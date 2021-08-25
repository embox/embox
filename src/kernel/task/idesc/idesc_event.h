/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
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
 * @brief Prepare link to wait on idesc. It also checks for O_NONBLOCK of
 * descriptor, and return -EAGAIN if it set, but link still is ready to
 * wait and should be cleanup'ed before link freeing.
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
 * @brief Cleanup idesc_wait_link
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

/* TODO mask is unused, and not sure if sometime will. This is called from
 * object's operation which can't continue until some condition occur. Even
 * if this is successfuly worked, it is not unlikely that operation still can't
 * run and this is called again. For example, input feed to tty, causing
 * reading process to wake, handle input and sleep again because it's ICANON
 * and no CR arrived yet.
 */
/**
 * @brief Wait for something to occur on idesc. Called from locked context
 * (for idesc), which means nothing could happen initially. Expressions for
 * leaving and entering locked conntext passed as args, illuminating race
 * condition possibility
 *
 * @param _unlock_expr Unlock exrepssion, unblocking possible idesc_notify
 * @param _idesc Idesc to wait on
 * @param _iwl Preallocated idesc_wait_link
 * @param _mask Mask of events to wait (constructed from POLL* flags)
 * @param _timeout Timeout
 * @param _lock_expr Lock expression, returing back to inital locked context
 *
 * @return 0 on some notify occured
 * @return Negating on error
 */
#define IDESC_WAIT_LOCKED(_unlock_expr, _idesc, _iwl, _mask, _timeout, _lock_expr) \
	({                                                   \
	 	int __res;                                       \
		idesc_wait_init(_iwl, _mask);                    \
                                                         \
		threadsig_lock();                                \
		__res = idesc_wait_prepare(_idesc, _iwl);        \
                                                         \
		if (!__res) {                                    \
			_unlock_expr;                                \
			__res = sched_wait_timeout(_timeout, NULL);  \
			_lock_expr;                                  \
		}                                                \
                                                         \
		idesc_wait_cleanup(_idesc, _iwl);                \
		threadsig_unlock();                              \
	 	__res;                                           \
	})


#endif /* IDESC_EVENT_H_ */
