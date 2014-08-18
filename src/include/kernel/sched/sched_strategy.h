/**
 * @file
 * @brief Defines the simplest operations in scheduler. Implementation depends
 * on the scheduling algorithm. Part of method is used in other functions.
 * All of policy must implement its interface.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Bulychev Anton
 *          - Description of functions
 */

#ifndef KERNEL_SCHED_SCHED_STRATEGY_H_
#define KERNEL_SCHED_SCHED_STRATEGY_H_

#include <module/embox/kernel/sched/strategy/api.h>

#include <kernel/sched/sched_priority.h>

struct schedee;
struct thread;

struct runq;				/* Queue of running threads  */

#if 0
/**
 * Finilize queue of running threads.
 *
 * @param runq
 *   Running queue.
 */
extern void runq_fini(struct runq *runq);
#endif

/**
 * Returns pointer to the currently executing thread.
 *
 * @param runq
 *   Running queue.
 */
extern struct thread *runq_current(struct runq *runq);


#endif /* KERNEL_SCHED_SCHED_STRATEGY_H_ */
