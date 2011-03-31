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

/** Thread states */
enum thread_state {
	/** Thread execution is blocked due thread waits for event. */
	THREAD_STATE_WAIT = (0x1 << 0),
	/** Thread has been halt for execution by another thread. */
	THREAD_STATE_SUSP = (0x1 << 1),
	/** Thread has been halt and waits event at the same time. */
	THREAD_STATE_WAIT_SUSP = (0x1 << 0) | (0x1 << 1),
	/** Thread is currently executing. */
	THREAD_STATE_RUNNING,
	/**  Thread is either not yet executing or has already finished. */
	THREAD_STATE_TERMINATE,
};

#define __THREAD_STATE_TOTAL 5

/** Action that can be execute to the thread. They lead to change thread state. */
enum thread_state_action {
	THREAD_STATE_ACTION_START = 0,
	THREAD_STATE_ACTION_STOP,
	THREAD_STATE_ACTION_SUSPEND,
	THREAD_STATE_ACTION_RESUME,
	THREAD_STATE_ACTION_SLEEP,
	THREAD_STATE_ACTION_WAKE,
	THREAD_STATE_ACTION_RUN,
	THREAD_STATE_ACTION_LEAVE_CPU,
};

#define __THREAD_STATE_ACTION_TOTAL 8

/**
 * Allows to determine validity of take the action of thread with given state.
 * @param state
 *   Current state of thread.
 * @param action
 *   Anticipated or perpetrated action.
 * @return
 *   The thread state value after action.
 */
inline static enum thread_state thread_state_transition(
		enum thread_state state, enum thread_state_action action) {
	extern const enum thread_state
			__thread_state_transition_table[__THREAD_STATE_TOTAL ][__THREAD_STATE_ACTION_TOTAL ];
	assert(state);
	return __thread_state_transition_table[state - 1][action];
}

#endif /* KERNEL_THREAD_STATE_H_ */
