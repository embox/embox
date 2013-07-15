/**
 * @file
 * @brief
 *
 * @date 13.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_PRIORITY_H_
#define KERNEL_THREAD_PRIORITY_H_

#define THREAD_PRIORITY_MIN 0   /**< The lowest priority is 0. */
#define THREAD_PRIORITY_MAX 255 /**< The highest priority. */

/** Total amount of valid priorities. */
#define THREAD_PRIORITY_TOTAL \
	(THREAD_PRIORITY_MAX - THREAD_PRIORITY_MIN + 1)

#define THREAD_PRIORITY_NORMAL \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_MAX) / 2

#define THREAD_PRIORITY_LOW  \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_NORMAL) / 2
#define THREAD_PRIORITY_HIGH \
	(THREAD_PRIORITY_MAX + THREAD_PRIORITY_NORMAL) / 2

#define THREAD_PRIORITY_DEFAULT \
	THREAD_PRIORITY_NORMAL

typedef short __thread_priority_t;


struct thread_priority {
	__thread_priority_t   initial_priority; /**< Scheduling priority. */
	__thread_priority_t   sched_priority;   /**< Current scheduling priority. */
};

extern int thread_priority_set(struct thread *, __thread_priority_t new_priority);
extern __thread_priority_t thread_priority_get(struct thread *);
extern __thread_priority_t thread_priority_inherit(struct thread *t, __thread_priority_t priority);
extern __thread_priority_t thread_priority_reverse(struct thread *t);

#endif /* KERNEL_THREAD_PRIORITY_H_ */
