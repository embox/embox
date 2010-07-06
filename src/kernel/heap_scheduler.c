/**
 * @file
 * @brief Implementation of methods in scheduler_base.h
 *
 * @date 1.07.2010
 * @author Avdyukhin Dmitry
 */
#ifndef _HEAP_SCHEDULER_
#define _HEAP_SCHEDULER_

#include <lib/list.h>
#include <kernel/scheduler.h>
#include <kernel/thread_heap.h>

void _scheduler_start(void) {
	/* Nothing to do. */
}

void _scheduler_add(struct thread *added_thread) {
	heap_insert(added_thread);
}

void _scheduler_init(void) {
	/* Nothing to do. */
}

struct thread *_scheduler_next(struct thread *prev_thread) {
	struct thread *next_thread;
	if (prev_thread->state == THREAD_STATE_RUN) {
		heap_insert(prev_thread);
	}
	next_thread = heap_extract();
	next_thread->reschedule = false;
	return next_thread;
}

void _scheduler_remove(struct thread *removed_thread) {
	if (heap_contains(removed_thread)) {
		heap_delete(removed_thread);
	}
}

#endif
