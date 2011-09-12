/**
 * @file
 * @brief TODO documentation for sched_impl.h -- Alina
 *
 * @date 23.03.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_IMPL_H_
#define KERNEL_THREAD_SCHED_IMPL_H_

struct event;

static inline void sched_wake(struct event *e) {
	extern void __sched_wake(struct event *e, int wake_all);
	__sched_wake(e, 1);
}

static inline void sched_wake_one(struct event *e) {
	extern void __sched_wake(struct event *e, int wake_all);
	__sched_wake(e, 0);
}

#endif /* KERNEL_THREAD_SCHED_IMPL_H_ */
