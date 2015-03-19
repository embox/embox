#include <util/err.h>
#include <hal/arch.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

static void *idle_run(void *arg) {
	arch_idle();
	sched_wakeup(schedee_get_current());
	return NULL;
}

int idle_thread_create(void) {
	struct lthread *lt;

	lt = lthread_create(idle_run, NULL);
	if (0 != err(lt)) {
		return err(lt);
	}

	sched_change_priority(&lt->schedee, SCHED_PRIORITY_MIN);
#if 0 //XXX
	cpu_init(cpu_get_id(), t);
#else
	sched_affinity_set(&lt->schedee.affinity, 1 << cpu_get_id());
#endif
	lthread_launch(lt);

	return 0;
}
