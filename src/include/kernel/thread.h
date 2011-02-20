/**
 * @file
 * @brief Defines structure of threads and methods,
 * which allows working with them.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 *
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <hal/context.h>
#include <lib/list.h>
#include <queue.h>
#include <string.h>
#include <util/macro.h>
#include <util/guard.h>

#define THREAD_POOL_SIZE 0x100

typedef int thread_id_t;
typedef int thread_priority_t;
typedef enum {
	THREAD_STATE_RUN,
	THREAD_STATE_WAIT,
	THREAD_STATE_STOP,
	THREAD_STATE_ZOMBIE
} thread_state_t;

typedef int msg_t;
typedef void * msg_data_t;

/**
 * Structure which describes events.
 * Now contains just a list of associated threads.
 */
struct event {
	struct list_head threads_list;
};

/**
 * Structure, describing messages, sent to threads.
 */
struct message {
	/* Message type. */
	msg_t type;
	/* Information in message. */
	msg_data_t data;
	/* Node in queue of messages. */
	struct list_head list;
};

/**
 * Thread, which makes nothing.
 * Is used to be working when there is no another process.
 */
extern struct thread *idle_thread;

/**
 * Thread, which is working now.
 */
extern struct thread *current_thread;

struct pprocess;
/**
 * Structure, describing threads.
 */
struct thread {
	/** Context of thread. */
	struct context context;
	/** Function, running in thread. */
	void (*run)(void);
	/** Does thread exist? (For it memory was alloted and it was not deleted) */
	bool exist;
	/** Flag, which shows, whether tread can be changed. */
	bool reschedule;
	/** Thread's identifier. Unique for each thread. */
	thread_id_t id;
	/** Thread's priority among another threads. */
	thread_priority_t priority;
	/** State, in which thread is now. */
	thread_state_t state;
	/** Shows if thread is waiting for message. */
	bool need_message;
	/** Queue of messages, sent to this thread. */
	queue_t messages;
	/** Event, appearing when thread receives message. */
	struct event msg_event;
	/** List item, corresponding to thread in list of some event. */
	struct list_head wait_list;

	/*----- Scheduler-dependent fields -------*/

	/* List and priorlist. */
	/** List item, corresponding to thread in list of executed threads. */
	struct list_head sched_list;

	/* Heap. */
	/** Index of thread in heap. */
	int heap_index;
	/**
	 * For each priority there is a state.
	 * We can start a thread iff his run_count equals to
	 * priority_state of his priority or there is no threads
	 * with the same priority and "right" run_count.
	 * Is needed for heap_scheduler.
	 */
	bool run_count;
	/* Pseudo process */
	#ifdef CONFIG_PP_ENABLE
	struct pprocess *pp;
	#endif
};

/**
 * Creates new thread
 *
 * @param run function executed in created thread.
 * @param stack_address address of thread's stack.
 * @return pointer to new thread if all parameters are correct.
 * @return NULL if one of parameters is NULL or all places for threads are occupied.
 */
extern struct thread *thread_create(void (*run)(void), void *stack_address);

/**
 * Starts a thread.
 */
extern void thread_start(struct thread *thread);

/**
 * Changes thread's priority.
 */
extern void thread_change_priority(struct thread *thread, int new_priority);

/**
 * Stops chosen thread.
 * Deletes previous zombie.
 * Makes it a zombie.
 */
extern int thread_stop(struct thread *stopped_thread);

/**
 * Switches context to another thread.
 * Currently working thread leaves CPU for some time.
 */
extern void thread_yield(void);

/**
 * Send message to thread.
 *
 * @param message sent message
 * @param thread thread to receive message
 */
extern void msg_send(struct message *message, struct thread *thread);

/**
 * Allows thread to receive a message.
 *
 * @param thread thread, which wants to take a message.
 * @return received message (wait until there will be such a message).
 */
extern struct message *msg_receive(void);

/**
 * Allots memory for new message to send to thread.
 *
 * @return pointer to new message.
 */
extern struct message *msg_new(void);

/**
 * Free memory from the message.
 *
 * @param message deleted message.
 * @return -1 if message is NULL
 * @return 0 otherwise.
 */
extern int msg_erase(struct message *message);

#define thread_foreach(t) MACRO_INVOKE(__thread_foreach_iterator, \
		t, GUARD_SUFFIX(__thread_iterator))

#define __thread_foreach_iterator(t, i) \
	for (struct thread_iterator i##_alloc, *i = thread_get_all(&i##_alloc); \
		(t = thread_iterator_has_next(i) ? thread_iterator_next(i) : NULL);)

struct thread_iterator {
	struct thread *current;
};

inline static struct thread_iterator *thread_get_all(struct thread_iterator *iterator) {
	extern struct thread __thread_pool[];
	if (NULL == iterator) {
		return NULL;
	}
	iterator->current = __thread_pool;
	return iterator;
}

inline static bool thread_iterator_has_next(struct thread_iterator *iterator) {
	extern struct thread __thread_pool[];
	return NULL != iterator && iterator->current <
			__thread_pool + THREAD_POOL_SIZE;
}

inline static struct thread *thread_iterator_next(struct thread_iterator *iterator) {
	if (!thread_iterator_has_next(iterator)) {
		return NULL;
	}
	return iterator->current++;
}

#ifdef CONFIG_PP_ENABLE
#include <kernel/pp.h>
#endif

#endif /* THREAD_H_ */
