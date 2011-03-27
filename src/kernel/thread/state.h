/**
 * @file
 * @brief TODO
 *
 * @date 28.03.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_STATE_H_
#define KERNEL_THREAD_STATE_H_

#include <assert.h>

/** TODO */
enum thread_state {
	THREAD_STATE_WAIT = (0x1 << 0),
	THREAD_STATE_SUSP = (0x1 << 1),
	THREAD_STATE_WAIT_SUSP = (0x1 << 0) | (0x1 << 1),
	THREAD_STATE_READY,
	THREAD_STATE_RUNNING,
	THREAD_STATE_TERMINATE,
};

#define __THREAD_STATE_TOTAL 6

/** TODO */
enum thread_state_action {
	THREAD_STATE_ACTION_START = 0,
	THREAD_STATE_ACTION_STOP,
	THREAD_STATE_ACTION_SUSPEND,
	THREAD_STATE_ACTION_RESUME,
	THREAD_STATE_ACTION_SLEEP,
	THREAD_STATE_ACTION_WAKE,
	THREAD_STATE_ACTION_RUN,
	THREAD_STATE_ACTION_PREEMPT,
};

#define __THREAD_STATE_ACTION_TOTAL 8

inline static enum thread_state thread_state_transition(
		enum thread_state state, enum thread_state_action action) {
	extern const enum thread_state
			__thread_state_transition_table[__THREAD_STATE_TOTAL ][__THREAD_STATE_ACTION_TOTAL ];
	assert(state);
	return __thread_state_transition_table[state - 1][action];
}

#endif /* KERNEL_THREAD_STATE_H_ */
