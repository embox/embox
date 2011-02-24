/**
 * @file
 * @brief Threads internal implementation.
 *
 * @date 24.02.2011
 * @author Alina Kramar
 */

#ifndef THREAD_H_
# error "Do not include this file directly, use <kernel/thread.h> instead!"
#endif /* THREAD_H_ */

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
	__thread_id_t id;
	/** Thread's priority among another threads. */
	__thread_priority_t priority;
	/** State, in which thread is now. */
	enum thread_state state;
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


#define THREAD_POOL_SZ 0x100

#define __thread_foreach(t) MACRO_INVOKE(__thread_foreach_iterator, \
		t, GUARD_SUFFIX(__thread_iterator))

#define __thread_foreach_iterator(t, i) \
	for (struct __thread_iterator i##_alloc, *i = __thread_get_all(&i##_alloc); \
		(t = __thread_iterator_has_next(i) ? __thread_iterator_next(i) : NULL);)

struct __thread_iterator {
	struct thread *next;
};

inline static bool __thread_iterator_has_next(struct __thread_iterator *iterator) {
	extern struct thread __thread_pool[];
	return NULL != iterator && iterator->next < __thread_pool
			+ THREAD_POOL_SZ;
}

inline static void __thread_iterator_skip_holes(
		struct __thread_iterator *iterator) {
	while (__thread_iterator_has_next(iterator) && !iterator->next->exist) {
		iterator->next++;
	}
}

inline static struct __thread_iterator *__thread_get_all(
		struct __thread_iterator *iterator) {
	extern struct thread __thread_pool[];
	if (NULL == iterator) {
		return NULL;
	}
	iterator->next = __thread_pool;
	__thread_iterator_skip_holes(iterator);
	return iterator;
}

inline static struct thread *__thread_iterator_next(
		struct __thread_iterator *iterator) {
	struct thread *result;
	if (!__thread_iterator_has_next(iterator)) {
		return NULL;
	}
	result = iterator->next++;
	__thread_iterator_skip_holes(iterator);
	return result;
}

inline static struct thread *thread_get_by_id(__thread_id_t id) {
	extern struct thread __thread_pool[];
	struct thread *thread = __thread_pool + id;

	if (!(0 <= id && id < THREAD_POOL_SZ)) {
		return NULL;
	}

	return thread->exist ? thread : NULL;
}
