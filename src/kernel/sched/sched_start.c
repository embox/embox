/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.08.2014
 */

#include <assert.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <module/embox/kernel/stack.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/task.h>

extern struct thread *idle_thread_create(void);

#define STACK_SZ \
	((size_t) OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size))

EMBOX_UNIT_INIT(sched_start_init);

static void *boot_stub(void *arg) {
	panic("Entering boot_stub");
}

static struct thread *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;

	bootstrap = thread_init_stack(&_stack_top - STACK_SZ, STACK_SZ,
			THREAD_PRIORITY_NORMAL, boot_stub, NULL);

	task_set_main(task_kernel_task(), bootstrap);

	return bootstrap;
}

static int sched_start_init(void) {
	struct thread *current;
	struct thread *idle;
	int err;

	current = boot_thread_create(); /* 'init' thread ID=1 */
	assert(current != NULL);
	err = sched_init(&current->schedee);

	if (err == 0) {
		idle = idle_thread_create(); /* idle thread always has ID=0 */
		assert(idle != NULL);
	}

	return err;
}
