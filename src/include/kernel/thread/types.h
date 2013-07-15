/**
 * @file
 * @brief TODO documentation for types.h -- Alina
 *
 * @date 23.03.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_TYPES_H_
#define KERNEL_THREAD_TYPES_H_

#include <stddef.h>
#include <sys/types.h>


typedef int __thread_id_t;
typedef unsigned int __thread_state_t;

#include <hal/context.h>

#include <kernel/sched/sched_strategy.h>
//#include <linux/list.h>
#include <util/dlist.h>
#include <kernel/sched/sched_priority.h>
#include <kernel/thread/thread_priority.h>
#include <kernel/thread/wait_data.h>


struct runq;
struct sleepq;

struct thread {
	__thread_state_t   state;         /**< Current state. */

	struct context     context;       /**< Architecture-dependent CPU state. */

	void            *(*run)(void *);  /**< Start routine. */
	union {
		void          *run_arg;       /**< Argument to pass to start routine. */
		void          *run_ret;       /**< Return value of the routine. */
		void          *join_ret;      /**< Exit value of a join target. */
	} /* unnamed */;
	void              *stack;         /**< Allocated thread stack buffer. */
	size_t             stack_sz;      /**< Stack size. */


	union {
		struct runq    *runq;      /**< For running/ready state. */
		struct sleepq  *sleepq;    /**< For sleeping state. */
	} /* unnamed */;

	__thread_id_t      id;            /**< Unique identifier. */

	struct dlist_head  thread_link;   /**< Linkage on all threads. */

	struct thread_priority thread_priority;

	struct thread     *joined;        /**< Thread which joined to this. */

	struct wait_data   wait_data;

	struct task       *task;          /**< Task belong to. */
	struct dlist_head  task_link;     /**< Link in list holding task threads. */

	clock_t            running_time;  /**< Running time of thread in clocks. */
	clock_t            last_sync;     /**< Last recalculation of running time. */

	struct sched_strategy_data sched;/**< Scheduler-private data. */

	struct mutex      *mutex_waiting; /**< Mutex we are waiting for (if any). */

	unsigned int       affinity;      /**< CPU affinity of the thread. */
};

#endif /* KERNEL_THREAD_TYPES_H_ */
