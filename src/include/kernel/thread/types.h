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

#include <kernel/thread/common_types.h>
#include <hal/context.h>
#include <hal/cpu.h>

#include <kernel/spinlock.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_cancel.h>
#include <kernel/sched.h>
#include <kernel/thread/thread_wait.h>

#include <util/dlist.h>

struct task;

/* Resource mgmt flags. */
#define TS_INIT         (0x0)
#define TS_LAUNCHED     (0x1 << 0)
#define TS_EXITED       (0x1 << 1)
#define TS_DETACHED     (0x1 << 2)

/**
 * Thread control block.
 */
struct thread {
	struct schedee    schedee;     /**< Schedee interface for scheduler */

	void              *(*run)(void *); /**< Start routine */
	void              *run_arg;        /**< Argument to be passed to run */

	unsigned int       critical_count;
	unsigned int       siglock;

	unsigned int       state;        /**< Thread-specific state. */

	struct context     context;      /**< Architecture-dependent CPU state. */

	union {
		void          *run_ret;      /**< Return value of the routine. */
		void          *joining;      /**< A joining thread (if any). */
	} /* unnamed */;

	thread_stack_t     stack;        /**< Handler for work with thread stack */

	thread_id_t        id;           /**< Unique identifier. */

	struct task       *task;         /**< Task belong to. */
	struct dlist_head  thread_link;  /**< list's link holding task threads. */

	struct sigstate    sigstate;     /**< Pending signal(s). */

	thread_local_t     local;
	thread_cancel_t    cleanups;

	struct thread_wait thread_wait;

	int                policy;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
