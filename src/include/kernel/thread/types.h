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

#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_cancel.h>
#include <kernel/sched/waitq.h>

#include <util/dlist.h>

#define __THREAD_STATE_READY     (0x1 << 0) /* present in runq */
#define __THREAD_STATE_WAITING   (0x1 << 1) /* waiting for an event */
#define __THREAD_STATE_RUNNING   (0x1 << 2) /* executing on CPU now */

/* Resource mgmt flags. */
#define __THREAD_STATE_DETACHED  (0x1 << 4)

/* zombie */
#define __THREAD_STATE_DO_EXITED(state)                        \
	state & ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING |  \
			__THREAD_STATE_RUNNING)
#define __THREAD_STATE_IS_EXITED(state)                        \
	!(state & (__THREAD_STATE_READY | __THREAD_STATE_WAITING | \
			__THREAD_STATE_RUNNING))

typedef int __thread_id_t;

struct task;

struct thread {
	unsigned int       state;          /**< Current state. */

	struct context     context;      /**< Architecture-dependent CPU state. */

	void            *(*run)(void *); /**< Start routine. */
	void              *run_arg;      /**< Argument to pass to start routine. */
	union {
		void          *run_ret;      /**< Return value of the routine. */
		void          *join_wq;      /**< A queue of joining threads. */
	} /* unnamed */;

	thread_stack_t     stack;        /**< Handler for work with thread stack */

	__thread_id_t      id;           /**< Unique identifier. */

	struct task       *task;         /**< Task belong to. */
	struct dlist_head  thread_link;  /**< list's link holding task threads. */

	struct wait_link   *wait_link;   /**< Hold data in waiting mode */

	struct sigstate    sigstate;     /**< Pending signal(s). */

	struct sched_attr  sched_attr;   /**< Scheduler-private data. */
	int                policy;       /**< Scheduling policy*/

	thread_local_t     local;
	thread_cancel_t    cleanups;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
