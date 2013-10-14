/**
 * @file
 *
 * @brief Describes thread descriptor structure
 *
 * @date 23.03.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_TYPES_H_
#define KERNEL_THREAD_TYPES_H_

#include <hal/context.h>

#include <kernel/thread/state.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/wait_data.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/thread/thread_local.h>

#include <util/dlist.h>


typedef int __thread_id_t;

struct task;

struct thread {
	thread_state_t     state;          /**< Current state. */

	struct context     context;      /**< Architecture-dependent CPU state. */

	void            *(*run)(void *); /**< Start routine. */
	union {
		void          *run_arg;      /**< Argument to pass to start routine. */
		void          *run_ret;      /**< Return value of the routine. */
		void          *join_ret;     /**< Exit value of a join target. */
	} /* unnamed */;

	thread_stack_t     stack;        /**< Handler for work with thread stack */

	__thread_id_t      id;           /**< Unique identifier. */

	struct task       *task;         /**< Task belong to. */
	struct dlist_head  thread_link;  /**< list's link holding task threads. */

	struct thread_resinfo resinfo;   /**< Resources info*/

	struct wait_data   wait_data;    /**< Hold data in waiting mode */

	struct sched_attr  sched_attr;   /**< Scheduler-private data. */

	int                policy;       /**< Scheduling policy*/

	thread_local_t local;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
