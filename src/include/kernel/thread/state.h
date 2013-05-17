/**
 * @file
 * @brief TODO
 *
 * @date 24.04.11
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_STATE_H_
#define KERNEL_THREAD_STATE_H_

#include <assert.h>
#include <stdbool.h>

#include <kernel/thread/types.h>

typedef __thread_state_t thread_state_t;

#define __THREAD_STATE_INIT       0
#define __THREAD_STATE_ACTIVE    (0x1 << 0)
#define __THREAD_STATE_SLEEPING  (0x1 << 1)
#define __THREAD_STATE_EXITED    (0x1 << 2)
#define __THREAD_STATE_DETACHED  (0x1 << 3)
#define __THREAD_STATE_ONCPU     (0x1 << 4)

static inline __thread_state_t thread_state_init(void) {
	return __THREAD_STATE_INIT;
}

static inline bool thread_state_active(__thread_state_t state) {
	return state & __THREAD_STATE_ACTIVE;
}

static inline bool thread_state_sleeping(__thread_state_t state) {
	return state & __THREAD_STATE_SLEEPING;
}

static inline bool thread_state_exited(__thread_state_t state) {
	return state & __THREAD_STATE_EXITED;
}

static inline bool thread_state_detached(__thread_state_t state) {
	return state & __THREAD_STATE_DETACHED;
}

static inline bool thread_state_oncpu(__thread_state_t state) {
	return state & __THREAD_STATE_ONCPU;
}

static inline bool thread_state_started(__thread_state_t state) {
	return thread_state_active(state) || thread_state_exited(state);
}

static inline bool thread_state_running(__thread_state_t state) {
	return thread_state_active(state) && !thread_state_sleeping(state)
			&& !thread_state_exited(state);
}

static inline bool thread_state_dead(__thread_state_t state) {
	return !thread_state_active(state) && thread_state_exited(state)
			&& thread_state_detached(state);
}


static inline __thread_state_t thread_state_do_activate(__thread_state_t state) {
	assert(!thread_state_started(state));
	return state | __THREAD_STATE_ACTIVE;
}

static inline __thread_state_t thread_state_do_sleep(__thread_state_t state) {
	assert(thread_state_running(state));
	return state | __THREAD_STATE_SLEEPING;
}

static inline __thread_state_t thread_state_do_wake(__thread_state_t state) {
	assert(thread_state_sleeping(state));
	return state & ~__THREAD_STATE_SLEEPING;
}

static inline __thread_state_t thread_state_do_exit(__thread_state_t state) {
	assert(!thread_state_exited(state));
	return ((state & ~__THREAD_STATE_ACTIVE) & ~__THREAD_STATE_SLEEPING)
			| __THREAD_STATE_EXITED;
}

static inline __thread_state_t thread_state_do_detach(__thread_state_t state) {
	assert(!thread_state_detached(state));
	return state | __THREAD_STATE_DETACHED;
}

static inline __thread_state_t thread_state_do_oncpu(__thread_state_t state) {
	assert(thread_state_running(state));
	return state | __THREAD_STATE_ONCPU;
}

static inline __thread_state_t thread_state_do_outcpu(__thread_state_t state) {
	assert(thread_state_oncpu(state));
	return state & (~__THREAD_STATE_ONCPU);
}

#endif /* KERNEL_THREAD_STATE_H_ */
