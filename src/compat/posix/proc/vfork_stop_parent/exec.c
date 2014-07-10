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

#include <kernel/task/resource/module_ptr.h>
#include <kernel/task/resource/task_argv.h>

extern int exec_call(void);

static void exec_trampoline(void) {
	sched_unlock();

	kill(task_get_id(task_get_parent(task_self())), SIGCONT);

	_exit(exec_call());
}

void *task_exec_callback(void *arg) {
	struct thread *t;
	struct context oldctx;

	sched_lock();
	t = thread_self();

	context_init(&t->context, true);
	context_set_entry(&t->context, exec_trampoline);
	context_set_stack(&t->context,
			thread_stack_get(t) + thread_stack_get_size(t));

	context_switch(&oldctx, &t->context);

	return NULL;
}
