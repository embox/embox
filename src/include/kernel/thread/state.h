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

/* thread state in scheduler state machine */
#define __THREAD_STATE_READY     (0x1 << 0) /* present in runq */
/* present in waitq or waiting for lunching*/
#define __THREAD_STATE_WAITING   (0x1 << 1)
#define __THREAD_STATE_RUNNING   (0x1 << 2) /* executing on CPU now */

/* zombie */
#define __THREAD_STATE_DO_EXITED(state)                        \
	state & ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING |  \
			__THREAD_STATE_RUNNING)
#define __THREAD_STATE_IS_EXITED(state)                        \
	!(state & (__THREAD_STATE_READY | __THREAD_STATE_WAITING | \
			__THREAD_STATE_RUNNING))

static inline int is_running(struct thread *t) {
	return t->state & __THREAD_STATE_RUNNING;
}

static inline int is_ready(struct thread *t) {
	return t->state & __THREAD_STATE_READY;
}

static inline int is_waiting(struct thread *t) {
	return t->state & __THREAD_STATE_WAITING;
}

static inline void make_ready(struct thread *t) {
	t->state |= __THREAD_STATE_READY;
	t->state &= ~__THREAD_STATE_RUNNING;
}

static inline void make_running(struct thread *t) {
	t->state |= __THREAD_STATE_RUNNING;
	t->state &= ~__THREAD_STATE_READY;
}

#endif /* KERNEL_THREAD_STATE_H_ */
