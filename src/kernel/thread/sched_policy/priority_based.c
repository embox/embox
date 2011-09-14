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

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

void sched_strategy_init(struct sched_strategy_data *s) {
	INIT_LIST_HEAD(&s->thread_link);
	s->is_priority_link = 0;
}

void runq_init(struct runq *rq, struct thread *current, struct thread *idle) {
	rq->current = current;
	INIT_LIST_HEAD(&rq->priority_list);
	runq_start(rq, idle);
}

int runq_start(struct runq *rq, struct thread *t) {
	thread_priority_t priority = t->priority;
	struct sched_strategy_data *td = &t->sched;

	struct thread *next = NULL;
	struct sched_strategy_data *next_data;

	assert(rq && t);
	assert(!list_empty(&rq->priority_list));

	list_for_each_entry(next, &rq->priority_list, sched.priority_link) {
		if (next->priority <= priority) {
			break;
		}
	}
	assert(next != NULL);
	next_data = &next->sched;

	if (next->priority == priority) {
		list_add_tail(&td->thread_link, &next_data->thread_link);
//		td->p_priority_link = &next_data->priority_link;
		td->is_priority_link = 0;

	} else {
		list_add_tail(&td->priority_link, &next_data->priority_link);
		assert(td->is_priority_link);
		INIT_LIST_HEAD(&td->thread_link);

	}

	return (priority > rq->current->priority);
}

int runq_stop(struct runq *rq, struct thread *t) {
	struct sched_strategy_data *td = &t->sched;

	assert(rq && t);
	assert(!list_empty(&rq->priority_list));

	if (list_empty(&td->thread_link)) {
		/* The whole chain consists of a single thread. */
		assert(td->is_priority_link);
		list_del(&td->priority_link);

	} else {
		if (td->is_priority_link) {
			struct list_head *new_priority_link = td->thread_link.next;

			/* Replace priority link being deleted with a new one. */
			list_add(new_priority_link, &td->priority_link);
			list_del(&td->priority_link);
		}

		/* Remove it from the chain. */
		list_del(&td->thread_link);
	}

	return (t == rq->current);
}

int runq_wake(struct runq *rq, struct sleepq *sq, int wake_all) {
	return 0;
}
int runq_sleep(struct runq *rq, struct sleepq *sq) {
	return 0;
}

int runq_priority_changing(struct runq *rq, struct thread *t, int new_priority) {
	return 0;
}

int runq_switch(struct runq *rq) {
	return 0;
}

int sleepq_empty(struct sleepq *sq) {
	return 0;
}

void sleepq_priority_changing(struct sleepq *sq, struct thread *t,
        int new_priority) {
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
