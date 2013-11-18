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

#define __THREAD_STATE_READY     (0x1 << 0) /* present in runq */
#define __THREAD_STATE_WAITING   (0x1 << 1) /* waiting for an event */
#define __THREAD_STATE_RUNNING   (0x1 << 2) /* executing on CPU now */

/* zombie */
#define __THREAD_STATE_DO_EXITED(state)                        \
	state & ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING |  \
			__THREAD_STATE_RUNNING)
#define __THREAD_STATE_IS_EXITED(state)                        \
	!(state & (__THREAD_STATE_READY | __THREAD_STATE_WAITING | \
			__THREAD_STATE_RUNNING))

typedef unsigned int thread_state_t;

#endif /* KERNEL_THREAD_STATE_H_ */
