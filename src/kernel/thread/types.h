/**
 * @file
 * @brief TODO documentation for types.h -- Alina
 *
 * @date 23.03.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_TYPES_H_
#define KERNEL_THREAD_TYPES_H_

#include <lib/list.h>
#include <hal/context.h>
#include <kernel/thread/event.h>

#include "task.h"
#include "state.h"

typedef int __thread_id_t;
typedef int __thread_priority_t;

struct thread {

	struct context    context;      /**< Architecture-dependent CPU state. */

	void           *(*run)(void *); /**< Start routine. */
	__extension__ union {
		void         *run_arg;      /**< Argument to pass to start routine. */
		void         *run_ret;      /**< Return value of start routine. */
		void         *join_ret;     /**< Exit value of a join target. */
	};
	void             *stack;        /**< Allocated thread stack buffer. */
	size_t            stack_sz;     /**< Stack size. TODO unused. -- Eldar */

	__thread_id_t     id;           /**< Unique identifier. */
	struct list_head  thread_link;  /**< Linkage on all threads. */
	int               flags;        /**< Private flags. TODO merge with state. */
	int               susp_cnt;     /**< Depth of #thread_suspend() calls. */
	struct event      exit_event;   /**< Thread exit event. */

	__thread_priority_t priority;   /**< Scheduling priority. */
	enum thread_state state;        /**< Current scheduling state. */
	struct list_head  sched_list;   /**< Scheduler-private link. */

	bool              need_message; /**< Waiting for message. */
	struct list_head  messages;     /**< Messages sent to the thread. */
	struct event      msg_event;    /**< Thread receives a message. */

	struct task task;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
