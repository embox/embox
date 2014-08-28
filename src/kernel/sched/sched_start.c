/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.08.2014
 */

#include <err.h>
#include <assert.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

#include <module/embox/kernel/stack.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#define KERNEL_STACK_SZ \

EMBOX_UNIT_INIT(sched_start_init);

static void *boot_stub(void *arg) {
	panic("Entering boot_stub");
}

static struct thread *boot_thread_create(void) {
	struct thread *bootstrap;
	extern char _stack_top;
	const size_t kernel_stack_sz = OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size);

	bootstrap = thread_init_stack(&_stack_top - kernel_stack_sz, kernel_stack_sz,
			THREAD_PRIORITY_NORMAL, boot_stub, NULL);

	task_set_main(task_kernel_task(), bootstrap);

	return bootstrap;
}

static void *idle_run(void *arg) {
	arch_idle();
	sched_wakeup(schedee_get_current());
	return NULL;
}

static int idle_thread_create(void) {
	struct lthread *lt;

	lt = lthread_create(idle_run, NULL);
	if (0 != err(lt)) {
		return err(lt);
	}

	lthread_priority_set(lt, SCHED_PRIORITY_MIN);
#if 0 //XXX
	cpu_init(cpu_get_id(), t);
#else
	sched_affinity_set(&lt->schedee.affinity, 1 << cpu_get_id());
#endif
	sched_wakeup(&lt->schedee);

	return 0;
}

static int sched_start_init(void) {
	struct thread *current;
	int err;

	current = boot_thread_create(); /* 'init' thread ID=1 */
	assert(current != NULL);
	err = sched_init(&current->schedee);

	if (err == 0) {
		err = idle_thread_create(); /* idle thread always has ID=0 */
	}

	return err;
}
