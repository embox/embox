/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/thread.h>
#include <stdint.h>
#include <string.h>
#include <util/binalign.h>

void task_init(struct task *tsk, int id, const char *name,
		struct thread *main_thread, task_priority_t priority) {
	assert(tsk != NULL);
	assert(binalign_check_bound((uintptr_t)tsk, sizeof(void *)));

	tsk->tsk_id = id;

	strncpy(tsk->tsk_name, name, sizeof tsk->tsk_name - 1);
	tsk->tsk_name[sizeof tsk->tsk_name - 1] = '\0';

	tsk->tsk_main = main_thread;
	main_thread->task = tsk;

	tsk->tsk_priority = priority;

	tsk->tsk_clock = 0;

	task_resource_init(tsk);
}
