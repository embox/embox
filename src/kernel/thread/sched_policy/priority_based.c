/**
 * @file
 * @brief Preemptive priority scheduler based on multilevel queue.
 *
 * @date 22.04.10
 * @author Darya Dzendzik
 *          - Initial implementation
 * @author Alina Kramar
 *          - Fixing switch logic
 *          - Interface change
 *          - Introducing auxiliary functions for operating with queue
 *
 * @see tests/kernel/thread/sched_policy/priority_based_test.c
 */

#include <assert.h>
#include <stddef.h>

#include <lib/list.h>
#include <util/array.h>
#include <util/prioq.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

static void move_thread_to_another_q(struct prioq *pq, struct thread *t);
static void change_thread_priority(struct prioq *pq, struct thread *t,
		int new_priority);

static inline int thread_prio_comparator(struct prioq_link *first,
		struct prioq_link *second) {
	struct thread *t1 = prioq_element(first, struct thread, sched.pq_link);
	struct thread *t2 = prioq_element(second, struct thread, sched.pq_link);
	return t1->priority - t2->priority;
}

void runq_init(struct runq *rq, struct thread *current, struct thread *idle) {
	rq->current = current;
	prioq_init(&rq->pq);
	runq_start(rq, idle);
}

int runq_start(struct runq *rq, struct thread *t) {
	assert(rq && t);

	prioq_enqueue(t, thread_prio_comparator, &rq->pq, sched.pq_link);

	return (t->priority > rq->current->priority);
}

int runq_stop(struct runq *rq, struct thread *t) {
	int is_current;

	assert(rq && t);

	if (!(is_current = (t == rq->current))) {
		prioq_remove(t, thread_prio_comparator, sched.pq_link);
	}

	return is_current;
}

static void wakeup_thread(struct runq *rq, struct thread *t) {
	t->state = thread_state_do_wake(t->state);
	assert(thread_state_running(t->state));

	t->runq = rq;

	prioq_enqueue(t, thread_prio_comparator, &rq->pq, sched.pq_link);
}

int runq_wake(struct runq *rq, struct sleepq *sq, int wake_all) {
	int ret = 0;
	struct thread *t;

	assert(rq && sq);

	if (sleepq_empty(sq)) {
		return 0;
	}

	if ((t = prioq_dequeue(thread_prio_comparator, &sq->pq,
			struct thread, sched.pq_link))) {
		wakeup_thread(rq, t);
		ret = (t->priority > rq->current->priority);

	} else {
		t = prioq_dequeue(thread_prio_comparator, &sq->suspended,
				struct thread, sched.pq_link);
		assert(t != NULL);

		t->state = thread_state_do_wake(t->state);
		assert(thread_state_suspended(t->state));

	}

	if (wake_all) {
		while ((t = prioq_dequeue(thread_prio_comparator, &sq->pq,
				struct thread, sched.pq_link))) {
			wakeup_thread(rq, t);
		}

		while ((t = prioq_dequeue(thread_prio_comparator, &sq->suspended,
				struct thread, sched.pq_link))) {
			t->state = thread_state_do_wake(t->state);
			assert(thread_state_suspended(t->state));
		}
	}

	return ret;
}

void runq_sleep(struct runq *rq, struct sleepq *sq) {
	struct thread *current;

	assert(rq && sq);

	current = rq->current;
	assert(current->runq == rq);

	move_thread_to_another_q(&sq->pq, current);

	current->sleepq = sq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *rq, struct thread *t, int new_priority) {
	assert(rq && t);

	change_thread_priority(&rq->pq, t, new_priority);

	return (new_priority > rq->current->priority);
}

int runq_switch(struct runq *rq) {
	return 0;
}

int sleepq_empty(struct sleepq *sq) {
	assert(sq);
	return prioq_empty(&sq->pq) && prioq_empty(&sq->suspended);
}

void sleepq_change_priority(struct sleepq *sq, struct thread *t,
		int new_priority) {
	struct prioq *pq;

	assert(sq && t);

	pq = thread_state_suspended(t->state) ? &sq->suspended : &sq->pq;

	change_thread_priority(pq, t, new_priority);
}

void sleepq_on_suspend(struct sleepq *sq, struct thread *t) {
	assert(sq && t);

	move_thread_to_another_q(&sq->suspended, t);
}

void sleepq_on_resume(struct sleepq *sq, struct thread *t) {
	assert(sq && t);

	move_thread_to_another_q(&sq->pq, t);
}

static void move_thread_to_another_q(struct prioq *pq, struct thread *t) {
	struct prioq_link *link;

	assert(pq && t);

	link = &t->sched.pq_link;

	prioq_remove_link(link, thread_prio_comparator);
	prioq_enqueue_link(link, thread_prio_comparator, pq);
}

static void change_thread_priority(struct prioq *pq, struct thread *t,
		int new_priority) {
	struct prioq_link *link;

	assert(pq && t);

	link = &t->sched.pq_link;

	prioq_remove_link(link, thread_prio_comparator);
	t->priority = new_priority;
	prioq_enqueue_link(link, thread_prio_comparator, pq);
}

#if 0
/**
 * Structure priority in list.
 */
struct run_thread_list {
	/** Link within sorted priorities list. */
	struct list_head priority_link;

	/** List of threads with given priority. */
	struct list_head thread_list;
};

/**
 * Thread that runs now.
 */
static struct thread *current;

/**
 * Array of thread list ordered by priority
 * (index is priority of each thread contained in list).
 */
static struct run_thread_list priorities[THREAD_PRIORITY_TOTAL];

/**
 * Priority queue that contains queue of thread. Main queue is ordered by thread
 * priority. Internal queue is sorted by time of thread adding. Doesn't contain
 * the current thread. Head of run_queue is a thread which most appropriate for
 * execution of all of existing thread.
 */
static struct list_head run_queue;

/* Run queue manipulation methods. */
static void run_enqueue(struct thread *t);
static void run_push(struct thread *t);

static struct thread *run_dequeue(void);
static struct thread *run_peek(void);

static void run_insert_priority(struct run_thread_list *priority);

/**
 * Adds thread to the end of the run_queue without changing the ordering.
 * @param thread
 *   Thread to add.
 */
static void run_enqueue(struct thread *t) {
	struct run_thread_list *priority = priorities + t->priority;

	assert(list_empty(&t->sched_list));

	if (list_empty(&priority->priority_link)) {
		run_insert_priority(priority);
	}

	list_add_tail(&t->sched_list, &priority->thread_list);
}

/**
 * Returns a thread from the queue top and then removes it from there.
 * @return
 *   Queue top.
 */
static struct thread *run_dequeue(void) {
	struct thread *t = run_peek();

	if (!t) {
		return NULL;
	}

	list_del_init(&t->sched_list);
	return t;
}

/**
 * Adds thread to the head of the run_queue without changing the ordering.
 * @param thread
 *   Thread to add.
 */
static void run_push(struct thread *t) {
	struct run_thread_list *priority = priorities + t->priority;

	if (list_empty(&priority->priority_link)) {
		assert(t != current);
		run_insert_priority(priority);
	}

	list_add(&t->sched_list, &priority->thread_list);
}

/**
 * Returns a thread from the queue top without removing it from there.
 * @return
 *   Top of queue.
 */
static struct thread *run_peek(void) {
	struct run_thread_list *priority;

	assert(!list_empty(&run_queue));

	priority = list_entry(run_queue.next, struct run_thread_list, priority_link);
	if (list_empty(&priority->thread_list)) {
		return NULL;
	}

	return list_entry(priority->thread_list.next, struct thread, sched_list);
}

/**
 * Insert @a priority to run_queue if threads with given priority don't exist.
 * Doesn't change the right order.
 * @param priority
 *   Links to thread and priority queues.
 */
static void run_insert_priority(struct run_thread_list *priority) {
	struct run_thread_list *next_priority;

	assert(list_empty(&priority->priority_link));
	assert(!list_empty(&run_queue));

	list_for_each_entry(next_priority, &run_queue, priority_link) {
		if (next_priority < priority) {
			break;
		}
	}

	list_add_tail(&priority->priority_link, &next_priority->priority_link);
}

bool sched_policy_start(struct thread *t) {
	assert(thread_state_running(t->state));

	run_enqueue(t);

	return (t->priority > current->priority);
}

bool sched_policy_stop(struct thread *t) {
	struct run_thread_list *priority = priorities + t->priority;

	assert(thread_state_running(t->state));

	if (t != current) {
		list_del_init(&t->sched_list);
	}

	if (list_empty(&priority->thread_list)) {
		/* Remove link on list of threads with given priority */
		list_del_init(&priority->priority_link);
	}

	return t == current;
}

struct thread *sched_policy_switch(struct thread *t) {
	struct thread *next;

	assert(current != NULL);

	next = run_dequeue();

	if (thread_state_running(current->state)) {
		if (!next) {
			return current;
		}
		if (current->priority < next->priority) {
			/* Preemption */
			run_push(current);
		} else {
			/* Quantum expiration */
			run_enqueue(current);
		}
	}

	assert(next != NULL);
	assert(thread_state_running(next->state));

	current = next;

	return current;
}

struct thread *sched_policy_current(void) {
	return current;
}

int sched_policy_init(struct thread *_current, struct thread *idle) {
	struct run_thread_list *priority, *current_priority;

	assert(_current != NULL);
	assert(idle != NULL);

	/* Initialize necessary lists. */
	INIT_LIST_HEAD(&run_queue);
	array_foreach_ptr(priority, priorities, ARRAY_SIZE(priorities)) {
		INIT_LIST_HEAD(&(priority->thread_list));
		INIT_LIST_HEAD(&(priority->priority_link));
	}

	current_priority = priorities + _current->priority;
	/* Can't use run_insert_priority because it assumes that
	 * run_queue is not empty. */
	list_add(&current_priority->priority_link, &run_queue);

	current = _current;

	/* Add the idle thread in a usual manner. */
	sched_policy_start(idle);

	return 0;
}
#endif
