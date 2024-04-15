/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.12.2014
 */

#include <hal/platform.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched.h>

static struct lthread idle;

static int idle_run(struct lthread *self) {
	platform_idle();
	lthread_launch(self);
	return 0;
}

int idle_thread_create(void) {
	lthread_init(&idle, idle_run);

	schedee_priority_set(&idle.schedee, SCHED_PRIORITY_MIN);
#if 0 //XX
	cpu_init(cpu_get_id(), t);
#else
	sched_affinity_set(&idle.schedee.affinity, 1 << cpu_get_id());
#endif
	lthread_launch(&idle);

	return 0;
}

int schedee_is_idle(struct schedee *s) {
	return &idle.schedee == s;
}
