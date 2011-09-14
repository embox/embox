/**
 * @file
 * @brief TODO documentation for priority_based.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SCHED_POLICY_PRIORITY_BASED_H_
#define KERNEL_THREAD_SCHED_POLICY_PRIORITY_BASED_H_

#include <lib/list.h>

#include <kernel/thread/sched_priority.h>

struct thread;

struct sched_strategy_data {

	/** Link within a headless chain of threads with the given priority. */
	struct list_head thread_link;

	__extension__ union {

		/** Link within priorities tier for a head of the threads chain. */
		struct list_head priority_link;

		/** For the rest threads in the chain. */
		__extension__ struct {
//			struct list_head *p_priority_link; /**< Pointer to the head. */
			void            *is_priority_link; /**< Contains @c NULL if not. */
		} /* unnamed */;

	} /* unnamed */;

};

struct runq {
	struct thread *current;
	struct list_head priority_list;
};

struct sleepq {
	struct list_head priority_list;
};

static inline struct thread *runq_current(struct runq *rq) {
	return rq->current;
}

#if 0
#define __sleepq_foreach(t, sleepq) \
	list_for_each_entry(t, &(sleepq)->priority_list, sched_list)
#else
#define __sleepq_foreach(t, sleepq) \
	for(t = NULL;;)
#endif

static inline void sleepq_init(struct sleepq *sleepq) {
	INIT_LIST_HEAD(&sleepq->priority_list);
}

#endif /* KERNEL_THREAD_SCHED_POLICY_PRIORITY_BASED_H_ */
