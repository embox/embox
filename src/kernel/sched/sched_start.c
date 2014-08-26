/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.08.2014
 */

#include <assert.h>
#include <embox/unit.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

EMBOX_UNIT_INIT(sched_start_init);

extern struct thread * boot_thread_create(void);
extern struct thread * idle_thread_create(void);

static int sched_start_init(void) {
	struct thread *current;
	struct thread *idle;

	current = boot_thread_create(); /* 'init' thread ID=1 */
	assert(current != NULL);

	idle = idle_thread_create(); /* idle thread always has ID=0 */
	assert(idle != NULL);

	return sched_init(&current->schedee);
}
