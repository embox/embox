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

#include <config/embox/kernel/thread/core.h>

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
#include <kernel/sched/waitq_protect_link.h>
#include <lib/libds/dlist.h>

struct task;

/* Resource mgmt flags. */
#define TS_INIT         (0x0)
#define TS_LAUNCHED     (0x1 << 0)
#define TS_EXITED       (0x1 << 1)
#define TS_DETACHED     (0x1 << 2)
/* Set by thread_delete(), so that a second deletion of the same thread finds
 * out rather than frees again. */
#define TS_DELETED      (0x1 << 3)

#define THREAD_DEFAULT_STACK_SIZE \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_size)

#define THREAD_STACK_ALIGN \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_align)

/**
 * Thread control block.
 */
/* Is this pointer a live thread?
 *
 * A thread that has been freed goes back to a pool whose next tenant is
 * memset to 0x53 -- and 0x53 has both TS_DETACHED and TS_EXITED in it, so a
 * use-after-free reads as a perfectly plausible thread state and asserts
 * somewhere that says nothing about the mistake. */
#define THREAD_MAGIC_LIVE 0x54485244u /* "THRD" */
#define THREAD_MAGIC_DEAD 0xdeadd00du

struct thread {
	unsigned int       magic;      /**< THREAD_MAGIC_LIVE while it exists */

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

	struct waitq_protect_link waitq_list;

	struct thread_wait thread_wait_list;

	int                policy;
} __attribute__ ((aligned(THREAD_STACK_ALIGN)));



#endif /* KERNEL_THREAD_TYPES_H_ */
