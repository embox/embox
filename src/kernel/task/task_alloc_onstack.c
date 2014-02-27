/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <kernel/task.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/panic.h>

struct task *task_alloc(struct task *task, size_t task_size) {
	void *addr;

	assert(task);
	assert(task->main_thread);

	addr = thread_stack_get(task->main_thread);

	if (thread_stack_reserved(task->main_thread, task_size) < 0) {
		panic("Too small thread stack size");
	}

	return addr;
}
