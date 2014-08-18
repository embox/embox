/**
 * @file
 *
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stdbool.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/time/timer.h>

#include <kernel/schedee/current.h>
#include <kernel/schedee/schedee.h>


void sched_wait_prepare_schedee(void) {
	struct schedee *s = schedee_get_current();

	// TODO SMP barrier? -- Eldar
	s->waiting = true;
}

void sched_wait_cleanup_schedee(void) {
	struct schedee *s = schedee_get_current();

	s->waiting = false;
	// TODO SMP barrier? -- Eldar
}

