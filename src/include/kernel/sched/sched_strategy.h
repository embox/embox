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

struct thread;

struct runq;				/* Queue of running threads  */

/**
 * Initializes scheduler strategy data of the thread.
 *
 * @param t
 *   Structure thread for which strategy data will initialize.
 */
extern void sched_strategy_init(struct thread *t);

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

/**
 * Switches current thread.
 *
 * @param runq
 *   Running queue.
 * @retval
 *   Thread to perform next. May equals to current.
 */
extern struct thread *runq_switch(struct runq *runq);


#endif /* KERNEL_SCHED_SCHED_STRATEGY_H_ */
