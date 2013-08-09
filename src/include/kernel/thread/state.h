/**
 * @file
 *
 * @brief Defines framework for thread states used for thread state machine.
 *
 * @date 24.04.11
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_STATE_H_
#define KERNEL_THREAD_STATE_H_

#include <assert.h>
#include <stdbool.h>

typedef unsigned int thread_state_t;

/* thread state in scheduler state machine */
#define __THREAD_STATE_INIT       0
#define __THREAD_STATE_ACTIVE    (0x1 << 0) /* present in runq */
#define __THREAD_STATE_SLEEPING  (0x1 << 1) /* waiting for any event */
#define __THREAD_STATE_ONCPU     (0x1 << 3) /* executing on CPU now */
#define __THREAD_STATE_EXITED    (0x1 << 2) /* zombie */

/* resources managed flag */
#define __THREAD_STATE_DETACHED  (0x1 << 4)

/** start value for thread machine state */
static inline thread_state_t thread_state_init(void) {
	return __THREAD_STATE_INIT;
}

/** checks whether thread state now in a scheduling mode */
static inline bool thread_state_active(thread_state_t state) {
	return state & __THREAD_STATE_ACTIVE;
}

/** checks whether thread state now in a waiting mode */
static inline bool thread_state_sleeping(thread_state_t state) {
	return state & __THREAD_STATE_SLEEPING;
}

/** checks whether thread finished */
static inline bool thread_state_exited(thread_state_t state) {
	return state & __THREAD_STATE_EXITED;
}

/** checks whether thread with the state executing now */
static inline bool thread_state_oncpu(thread_state_t state) {
	return state & __THREAD_STATE_ONCPU;
}


static inline bool thread_state_running(thread_state_t state) {
	return thread_state_active(state) && !thread_state_sleeping(state)
			&& !thread_state_exited(state);
}


static inline thread_state_t thread_state_do_activate(thread_state_t state) {
	assert(!thread_state_active(state));
	return state | __THREAD_STATE_ACTIVE;
}

static inline thread_state_t thread_state_do_sleep(thread_state_t state) {
	assert(thread_state_running(state));
	return state | __THREAD_STATE_SLEEPING;
}

static inline thread_state_t thread_state_do_wake(thread_state_t state) {
	assert(thread_state_sleeping(state));
	return state & ~__THREAD_STATE_SLEEPING;
}

static inline thread_state_t thread_state_do_exit(thread_state_t state) {
	assert(!thread_state_exited(state));
	return ((state & ~__THREAD_STATE_ACTIVE) & ~__THREAD_STATE_SLEEPING)
			| __THREAD_STATE_EXITED;
}

static inline thread_state_t thread_state_do_oncpu(thread_state_t state) {
	assert(thread_state_running(state));
	return state | __THREAD_STATE_ONCPU;
}

static inline thread_state_t thread_state_do_outcpu(thread_state_t state) {
	assert(thread_state_oncpu(state));
	return state & (~__THREAD_STATE_ONCPU);
}


static inline bool thread_state_detached(thread_state_t state) {
	return state & __THREAD_STATE_DETACHED;
}

static inline thread_state_t thread_state_do_detach(thread_state_t state) {
	assert(!thread_state_detached(state));
	return state | __THREAD_STATE_DETACHED;
}


#endif /* KERNEL_THREAD_STATE_H_ */
