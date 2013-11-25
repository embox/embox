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

#include <kernel/spinlock.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_cancel.h>

#include <util/dlist.h>

#define THREAD_STATE_INIT 0x0

#define THREAD_READY     (0x1 << 0)  /**< Not waiting (and not about to be). */
#define THREAD_ACTIVE    (0x1 << 1)  /**< Runs on CPU now (is current). */

/* Resource mgmt flags. */
#define __THREAD_STATE_EXITED    (0x1 << 3)
#define __THREAD_STATE_DETACHED  (0x1 << 4)

typedef int __thread_id_t;

struct task;

struct thread {
	unsigned int       state;          /**< Current state. */
	spinlock_t         lock;

	struct context     context;      /**< Architecture-dependent CPU state. */

	void            *(*run)(void *); /**< Start routine. */
	void              *run_arg;      /**< Argument to pass to start routine. */
	union {
		void          *run_ret;      /**< Return value of the routine. */
		void          *joining;      /**< A joining thread (if any). */
	} /* unnamed */;

	thread_stack_t     stack;        /**< Handler for work with thread stack */

	__thread_id_t      id;           /**< Unique identifier. */

	struct task       *task;         /**< Task belong to. */
	struct dlist_head  thread_link;  /**< list's link holding task threads. */

	struct sigstate    sigstate;     /**< Pending signal(s). */

	struct sched_attr  sched_attr;   /**< Scheduler-private data. */
	int                policy;       /**< Scheduling policy*/

	thread_local_t     local;
	thread_cancel_t    cleanups;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
