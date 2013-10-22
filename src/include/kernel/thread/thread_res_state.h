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
#define __THREAD_STATE_JOINABLE  0
#define __THREAD_STATE_JOINED    1
#define __THREAD_STATE_DETACHED  2

struct thread;

/* thread resources management information */
struct thread_res_state {
	/* Zero if joinable, otherwise has one of the resources managed flag */
	unsigned int      state;
	struct wait_queue *joined;  /**< Thread which joined to this. */
};

static inline void thread_res_state_get(struct thread_res_state *info, unsigned int *state) {
	*state = info->state;
}

static inline bool thread_res_state_detached(struct thread_res_state *info) {
	return info->state == __THREAD_STATE_DETACHED;
}

static inline bool thread_res_state_joined(struct thread_res_state *info) {
	return info->state == __THREAD_STATE_JOINED;
}

static inline thread_state_t thread_res_state_do_detach(struct thread_res_state *info) {
	assert(!thread_res_state_detached(info) && !thread_res_state_joined(info));
	return __THREAD_STATE_DETACHED;
}

static inline thread_state_t thread_res_state_do_join(struct thread_res_state *info) {
	assert(!thread_res_state_detached(info) && !thread_res_state_joined(info));
	return __THREAD_STATE_JOINED;
}

#endif /* RES_MANAGE_STATE_H_ */
