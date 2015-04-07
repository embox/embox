/**
 * @file
 * @brief Pseudo thread as boot thread. It cannot use sync functions.
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */

#include <kernel/panic.h>

#include <kernel/sched.h>
#include <kernel/sched/schedee_priority.h>

struct schedee boot_schedee;

static struct schedee *boot_process(struct schedee *prev, struct schedee *next) {
	assert(prev == next);
	assert(next == &boot_schedee);

	schedee_set_current(next);
	ipl_enable();

	return &boot_schedee;
}

struct schedee *boot_thread_create(void)
{
	schedee_init(&boot_schedee, SCHED_PRIORITY_HIGH, boot_process);
	return &boot_schedee;
};
