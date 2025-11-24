/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 * 	- shell invocation
 */

#include <string.h>
#include <unistd.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <hal/context.h>
#include <hal/vfork.h>

#include <kernel/task/resource/task_argv.h>

extern int exec_call(void);

static void exec_trampoline(void) {
	sched_unlock();

	kill(task_get_id(task_get_parent(task_self())), SIGCHLD);

	_exit(exec_call());
}

void *task_exec_callback(void *arg) {
	struct thread *t;

	sched_lock();
	t = thread_self();

	CONTEXT_JMP_NEW_STACK(exec_trampoline, thread_stack_get(t) + thread_stack_get_size(t));

	return NULL;
}

void *task_exit_callback(void *arg) {
	return arg;
}

