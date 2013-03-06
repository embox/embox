/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/thread.h>
#include <kernel/thread/sched_strategy.h>

static inline void queue_init(struct prior_queue *queue);
static inline void queue_insert(struct prior_queue *queue, struct thread *thread);
static inline void queue_remove(struct prior_queue *queue, struct thread *thread);
static inline struct thread *queue_peek(struct prior_queue *queue);
static inline struct thread *queue_extract(struct prior_queue *queue);
static inline int queue_empty(struct prior_queue *queue);

void sched_strategy_init(struct sched_strategy_data *s) {
	dlist_head_init(&s->link);
}

void runq_queue_init(runq_queue_t *queue) {
	queue_init(queue);
}

void runq_queue_insert(runq_queue_t *queue, struct thread *thread) {
	queue_insert(queue, thread);
}

void runq_queue_remove(runq_queue_t *queue, struct thread *thread) {
	queue_remove(queue, thread);
}

struct thread *runq_queue_extract(runq_queue_t *queue) {
	return queue_extract(queue);
}

void sleepq_queue_init(sleepq_queue_t *queue) {
	queue_init(queue);
}

void sleepq_queue_insert(sleepq_queue_t *queue, struct thread *thread) {
	queue_insert(queue, thread);
}

void sleepq_queue_remove(sleepq_queue_t *queue, struct thread *thread) {
	queue_remove(queue, thread);
}

struct thread *sleepq_queue_peek(sleepq_queue_t *queue) {
	return queue_peek(queue);
}

int sleepq_queue_empty(sleepq_queue_t *queue) {
	return queue_empty(queue);
}

/* QUEUE OPERATIONS */

static inline void queue_init(struct prior_queue *queue) {
	for (int i = THREAD_PRIORITY_MIN; i <= THREAD_PRIORITY_MAX; i++) {
		dlist_init(&queue->list[i]);
	}
}

static inline void queue_insert(struct prior_queue *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched.link, &queue->list[thread->priority]);
}

static inline void queue_remove(struct prior_queue *queue, struct thread *thread) {
	dlist_del(&thread->sched.link);
}

static inline struct thread *queue_peek(struct prior_queue *queue) {
	for (int i = THREAD_PRIORITY_MAX; i >= THREAD_PRIORITY_MIN; i--) {
		if (!dlist_empty(&queue->list[i])) {
			return dlist_entry(queue->list[i].next, struct thread, sched.link);
		}
	}

	return NULL;
}

static inline struct thread *queue_extract(struct prior_queue *queue) {
	struct thread *thread = queue_peek(queue);

	if (thread) {
		queue_remove(queue, thread);
	}

	return thread;
}

static inline int queue_empty(struct prior_queue *queue) {
	return queue_peek(queue) == NULL;
}

