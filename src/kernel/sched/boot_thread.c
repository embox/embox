/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */

#include <kernel/panic.h>

#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#include <module/embox/kernel/stack.h>

static void *boot_stub(void *arg) {
	panic("Entering boot_stub");
}

struct schedee *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;
	const size_t kernel_stack_sz = OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size);

	bootstrap = thread_init_stack(&_stack_top - kernel_stack_sz, kernel_stack_sz,
			SCHED_PRIORITY_NORMAL, boot_stub, NULL);

	task_set_main(task_kernel_task(), bootstrap);

	return &bootstrap->schedee;
}
