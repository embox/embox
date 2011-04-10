/**
 * @file
 * @brief TODO documentation for types.h -- Alina
 *
 * @date 23.03.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_TYPES_H_
#define KERNEL_THREAD_TYPES_H_

#include <drivers/vconsole.h>
#include <lib/list.h>
#include <hal/context.h>
#include <kernel/thread/event.h>
#include "state.h"

typedef int __thread_id_t;
typedef int __thread_priority_t;

#ifdef CONFIG_PP_ENABLE
struct pprocess;
#endif

struct thread {
	struct context    context;      /**< Architecture-dependent CPU state. */
	__thread_id_t     id;           /**< Unique identifier. */
	struct list_head  thread_link;  /**< Linkage on all threads. */
	void           *(*run)(void *); /**< Start routine. */
	__extension__ union {
		void         *run_arg;      /**< Argument to pass to start routine. */
		void         *run_ret;      /**< Return value of start routine. */
	};
	bool              reschedule;   /**< Whether rescheduling is needed. */
	__thread_priority_t priority;   /**< Scheduling priority. */
	enum thread_state state;        /**< Current state. */
	bool              need_message; /**< Waiting for message. */
	struct list_head messages;      /**< Messages sent to the thread. */
	struct event msg_event;         /**< Event, appearing when thread receives message. */
	/** Event which thread make after finish */
	struct event event;

	/*----- Scheduler-dependent fields -------*/

	/* List and priorlist. */
	/** List item, corresponding to thread in list of executed threads. */
	struct list_head sched_list;

	struct vconsole *own_console;

	/* Pseudo process */
#ifdef CONFIG_PP_ENABLE
struct pprocess *pp;
#endif

};

#endif /* KERNEL_THREAD_TYPES_H_ */
