/**
 * @file
 * @brief
 *
 * @date Mar 5, 2014
 * @author Anton Kozlov
 */
#include <embox/unit.h>

#include <kernel/thread.h>
#include <kernel/sched.h>


#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

EMBOX_UNIT_INIT(thread_core_init);

//extern struct thread *boot_thread_create(void);
extern struct thread *boot_thread_get(void);
static int thread_core_init(void) {
	struct thread *idle;
	struct thread *current;

	idle = cpu_get_idle(cpu_get_id()); /* idle thread always has ID=0 */
	//current = boot_thread_create(); /* 'init' thread ID=1 */
	current = boot_thread_get();

	return sched_init(idle, current);
}

