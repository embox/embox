/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <framework/mod/options.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/state.h>

#include <kernel/task.h>

#include <kernel/sched/runq.h>


void sched_strategy_init(struct thread *t) {
	runq_item_init(&(t->runnable.sched_attr.runq_link));
	sched_affinity_init(&(t->runnable));
	sched_timing_init(t);
}
