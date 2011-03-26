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

typedef int __thread_id_t;
typedef int __thread_priority_t;

/**
 * Structure which describes events.
 * Now contains just a list of associated threads.
 */
struct event {
	struct list_head threads_list;
};

enum thread_state {
	THREAD_STATE_RUN, THREAD_STATE_WAIT, THREAD_STATE_STOP, THREAD_STATE_ZOMBIE
};

#ifdef CONFIG_PP_ENABLE
struct pprocess;
#endif

struct thread {
	/** Thread's identifier. Unique for each thread. */
	__thread_id_t id;
	struct list_head thread_link;
	/** Context of thread. */
	struct context context;
	/** Function, running in thread. */
	void (*run)(void);
	/** Flag, which shows, whether tread can be changed. */
	bool reschedule;
	/** Thread's priority among another threads. */
	__thread_priority_t priority;
	/** State, in which thread is now. */
	enum thread_state state;
	/** Shows if thread is waiting for message. */
	bool need_message;
	/** Queue of messages, sent to this thread. */
	struct list_head messages;
	/** Event, appearing when thread receives message. */
	struct event msg_event;
	/** List item, corresponding to thread in list of some event. */
	struct list_head wait_list;

	/*----- Scheduler-dependent fields -------*/

	/* List and priorlist. */
	/** List item, corresponding to thread in list of executed threads. */
	struct list_head sched_list;

/* Pseudo process */
#ifdef CONFIG_PP_ENABLE
struct pprocess *pp;
#endif
};

#endif /* KERNEL_THREAD_TYPES_H_ */
