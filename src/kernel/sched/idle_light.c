#include <util/err.h>
#include <hal/arch.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

static struct lthread idle;

static void *idle_run(void *arg) {
	arch_idle();
	sched_wakeup(schedee_get_current());
	return NULL;
}

int idle_thread_create(void) {

	lthread_init(&idle, idle_run, NULL);

	sched_change_priority(&idle.schedee, SCHED_PRIORITY_MIN);
#if 0 //XX
	cpu_init(cpu_get_id(), t);
#else
	sched_affinity_set(&idle.schedee.affinity, 1 << cpu_get_id());
#endif
	lthread_launch(&idle);

	return 0;
}
