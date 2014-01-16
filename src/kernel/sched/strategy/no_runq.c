/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.01.2014
 */

#include <assert.h>
#include <kernel/sched/runq.h>

void runq_init(runq_t *queue) {
	*queue = NULL;
}

void runq_insert(runq_t *queue, struct thread *thread) {

	assert(!*queue);
	*queue = thread;
}

void runq_remove(runq_t *queue, struct thread *thread) {

	assert(*queue);
	*queue = NULL;
}

struct thread *runq_extract(runq_t *queue) {
	void *ret;

	assert(*queue);
	ret = *queue;
	*queue = NULL;

	return ret;
}
