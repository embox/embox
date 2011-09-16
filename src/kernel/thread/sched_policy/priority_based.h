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
#include <util/prioq.h>

#include <kernel/thread/sched_priority.h>

struct thread;

struct sched_strategy_data {
	struct prioq_link pq_link;
};

struct runq {
	struct thread *current;
	struct prioq pq;
};

struct sleepq {
	struct prioq pq;
};

static inline void sched_strategy_init(struct sched_strategy_data *s) {
	prioq_link_init(&s->pq_link);
}

static inline void sleepq_init(struct sleepq *sq) {
	prioq_init(&sq->pq);
}

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

#endif /* KERNEL_THREAD_SCHED_POLICY_PRIORITY_BASED_H_ */
