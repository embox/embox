/**
 * @file
 * @brief TODO documentation for priority_based.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_
#define KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_


#include <util/prioq.h>

#include <kernel/thread/startq.h>

struct thread;
struct sys_timer;

struct sched_strategy_data {
	struct prioq_link pq_link;
};

struct runq {
	struct prioq pq;

	struct sys_timer *tick_timer;
};

struct sleepq {
	struct prioq pq;
	struct startq_data startq_data;
};

static inline void sched_strategy_init(struct sched_strategy_data *s) {
	prioq_link_init(&s->pq_link);
}

static inline void sleepq_init(struct sleepq *sq) {
	prioq_init(&sq->pq);
	startq_init_sleepq(&sq->startq_data);
}

#endif /* KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_ */
