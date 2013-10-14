/**
 * @file
 *
 * @brief Defines framework for thread states used for thread resource
 * management.
 *
 * @date 14.10.13
 * @author Vita Loginova
 */

#ifndef RES_MANAGE_STATE_H_
#define RES_MANAGE_STATE_H_

/* resources managed flag */
#define __THREAD_STATE_JOINED    (0x1 << 0)
#define __THREAD_STATE_DETACHED  (0x1 << 1)

/* thread resources management information */
struct thread_resinfo {
	thread_state_t     state;   /**< Zero if joinable, otherwise has one of the
								resources managed flag*/
	struct thread     *joined;  /**< Thread which joined to this. */
};

static inline bool thread_state_detached(struct thread_resinfo *info) {
	return info->state & __THREAD_STATE_DETACHED;
}

static inline bool thread_state_joined(struct thread_resinfo *info) {
	return info->state & __THREAD_STATE_JOINED;
}

static inline thread_state_t thread_state_do_detach(struct thread_resinfo *info) {
	assert(!thread_state_detached(info) && !thread_state_joined(info));
	return info->state | __THREAD_STATE_DETACHED;
}

static inline thread_state_t thread_state_do_join(struct thread_resinfo *info) {
	assert(!thread_state_detached(info) && !thread_state_joined(info));
	return info->state | __THREAD_STATE_JOINED;
}

#endif /* RES_MANAGE_STATE_H_ */
