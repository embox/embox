/**
 * @file
 * @brief
 *
 * @date Mar 5, 2014
 * @author Anton Kozlov
 */

#include <assert.h>
#include <embox/unit.h>
#include <kernel/sched.h>

EMBOX_UNIT_INIT(thread_core_init);

extern struct thread * idle_thread_create(void);
extern struct thread * boot_thread_create(void);

static int thread_core_init(void) {
	struct thread *idle;
	struct thread *current;

	idle = idle_thread_create(); /* idle thread always has ID=0 */
	assert(idle != NULL);

	current = boot_thread_create(); /* 'init' thread ID=1 */
	assert(current != NULL);

	return sched_init(idle, current);
}
