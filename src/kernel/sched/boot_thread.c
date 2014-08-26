/**
 * @file
 *
 * @date Jul 18, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>

#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#include <module/embox/kernel/stack.h>

#define STACK_SZ \
	((size_t) OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size))

static void *boot_stub(void *arg) {
	assert(0, "Entering boot_stub");
	return NULL;
}

struct thread *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;

	bootstrap = thread_init_stack(&_stack_top - STACK_SZ, STACK_SZ,
			THREAD_PRIORITY_NORMAL, boot_stub, NULL);

	task_set_main(task_kernel_task(), bootstrap);

	return bootstrap;
}
