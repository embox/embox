/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */
#include <util/log.h>

#include <kernel/panic.h>

#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#include <hal/cpu.h>

static void *boot_stub(void *arg) {
	panic("Entering boot_stub");
	return 0;
}

extern void thread_set_current(struct thread *t);
struct schedee *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;

	bootstrap = thread_init_stack((void *) ((uintptr_t) &_stack_top - KERNEL_BSP_STACK_SZ), KERNEL_BSP_STACK_SZ,
			SCHED_PRIORITY_NORMAL, boot_stub, NULL);

	task_set_main(task_kernel_task(), bootstrap);
	thread_set_current(bootstrap);

	log_debug("boot_schedee = %#x", &bootstrap->schedee);

	return &bootstrap->schedee;
}
