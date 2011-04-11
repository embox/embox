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

enum thread_state {

	/** Thread execution is blocked because the thread waits for an event. */
	THREAD_STATE_SLEEPING = (0x1 << 0),

	/** Thread has been halted by another thread. */
	THREAD_STATE_SUSPENDED = (0x1 << 1),

	/** Thread has been both halted and blocked for an event. */
	THREAD_STATE_SLEEPING_SUSPENDED = (0x1 << 0) | (0x1 << 1),

	/** Thread is currently executing or is ready for that. */
	THREAD_STATE_RUNNING,

};

#define __THREAD_STATE_TOTAL 4

/**
 * Actions that can be performed on the thread.
 * Doing an action results in the state change.
 */
enum thread_state_action {
	THREAD_STATE_ACTION_SUSPEND,
	THREAD_STATE_ACTION_RESUME,
	THREAD_STATE_ACTION_SLEEP,
	THREAD_STATE_ACTION_WAKE,
};

#define __THREAD_STATE_ACTION_TOTAL 4

/**
 * Performs a state change accordingly to the specified @a action,
 * if it is valid for the given @a state.
 *
 * @param state
 *   Current state of thread.
 * @param action
 *   An action being performed on the thread.
 * @return
 *   The new state after doing the transition.
 * @retval 0
 *   Is the given @a action isn't applicable for the current state.
 */
inline static enum thread_state thread_state_transition(
		enum thread_state state, enum thread_state_action action) {
	extern const enum thread_state
			__thread_state_transition_table[__THREAD_STATE_TOTAL ][__THREAD_STATE_ACTION_TOTAL ];
	assert(state);
	return __thread_state_transition_table[state - 1][action];
}

inline static int thread_state_blocked(enum thread_state state) {
	int blocked = (0x1 << 0) | (0x1 << 1);
	return state & blocked;
}

#endif /* KERNEL_THREAD_STATE_H_ */
