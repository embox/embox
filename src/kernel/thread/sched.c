/**
 * @file
 * @brief TODO --Alina
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Kirill Skorodumov
 *          - Some insignificant contribution
 * @author Alina Kramar
 *          - Initial work on priority inheritance support
 * @author Eldar Abusalimov
 *          - Rewriting from scratch:
 *              - Interrupts safety, adaptation to Critical API
 *              - @c startq for deferred wake/resume processing
 */

#include <assert.h>
#include <errno.h>

#include <kernel/critical.h>
#include <kernel/irq_lock.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/current.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <kernel/task/signal.h>
#include <kernel/work.h>
#include <hal/context.h>
#include <hal/ipl.h>

#include <profiler/tracing/trace.h>

#include <time.h>

#include <kernel/cpu.h>

#include <util/member.h>

#include <embox/unit.h>

EMBOX_UNIT(unit_init, unit_fini);

static void post_switch_if(int condition);

static void sched_switch(void);

void sched_post_switch(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

static struct runq rq;
static struct work_queue startq;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

int sched_init(struct thread* current, struct thread *idle) {
	current->last_sync = clock();

	thread_set_current(current);

	runq_init(&rq, current, idle);

	assert(thread_state_started(current->state));
	assert(thread_state_started(idle->state));

	return 0;
}

void sched_start(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		assert(!thread_state_started(t->state));
		post_switch_if(runq_start(&rq, t));
		assert(thread_state_started(t->state));
	}
	sched_unlock();
}

void sched_finish(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		assert(!thread_state_exited(t->state));

		if (thread_state_running(t->state)) {
			post_switch_if(runq_finish(&rq, t));
		} else {
			if (thread_state_sleeping(t->state)) {
				sleepq_finish(t->sleepq, t);
			} else {
				t->state = thread_state_do_exit(t->state);
			}
		}

		assert(thread_state_exited(t->state));
	}
	sched_unlock();
}

static void do_wait_locked(void) {
	struct thread *current = sched_current();
	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	runq_sleep(&rq, NULL); /* TODO: Simple remove from runq */

	//assert(current->sleepq == sq);
	assert(thread_state_sleeping(current->state));

	post_switch_if(1);
}

static void timeout_handler(struct sys_timer *timer, void *sleep_data) {
	struct thread *thread = (struct thread *) sleep_data;
	sched_thread_notify(thread, -ETIMEDOUT);
}

static int notify_work(struct work *work) {
	struct wait_data *wait_data = member_cast_out(work, struct wait_data, work);
	struct thread *thread = member_cast_out(wait_data, struct thread, wait_data);

	assert(in_sched_locked());

	post_switch_if(sleepq_wake_thread(&rq, NULL, thread)); /* TODO: SMP */

	wait_data->on_notified(thread, wait_data->data);

	return 1;
}

void sched_thread_notify(struct thread *thread, int result) {
	irq_lock();
	{
		if (thread->wait_data.status == WAIT_DATA_STATUS_WAITING) {
			work_post(&thread->wait_data.work, &startq);

			thread->wait_data.status = WAIT_DATA_STATUS_NOTIFIED;
			thread->wait_data.result = result;

			critical_request_dispatch(&sched_critical);
		}
	}
	irq_unlock();
}

void sched_prepare_wait(notify_handler on_notified, void *data) {
	struct wait_data *wait_data = &sched_current()->wait_data;

	wait_data->data = data;
	wait_data->on_notified = on_notified;

	wait_data_prepare(wait_data, &notify_work);
}

void sched_cleanup_wait(void) {
	wait_data_cleanup(&sched_current()->wait_data);
}

int sched_wait_locked(unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = sched_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));
	assert(current->wait_data.status != WAIT_DATA_STATUS_NONE); /* Should be prepared */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	do_wait_locked();

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->wait_data.result;
}

int sched_wait(unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_wait_locked(timeout);
	}
	sched_unlock();

	return sleep_res;
}

void sched_post_switch(void) {
	sched_lock();
	{
		post_switch_if(1);
	}
	sched_unlock();
}

clock_t sched_get_running_time(struct thread *thread) {
	sched_lock();
	{
		if (thread_state_oncpu(thread->state)) {
			/* Recalculate time of the thread. */
			clock_t	new_clock = clock();
			thread->running_time += new_clock - thread->last_sync;
			thread->last_sync = new_clock;
		}
	}
	sched_unlock();

	return thread->running_time;
}

int sched_change_scheduling_priority(struct thread *thread,
		sched_priority_t new_priority) {
	assert((new_priority >= SCHED_PRIORITY_MIN)
			&& (new_priority <= SCHED_PRIORITY_MAX));

	sched_lock();
	{
		assert(!thread_state_exited(thread->state));

		if (thread_state_running(thread->state)) {
			post_switch_if(runq_change_priority(thread->runq, thread, new_priority));
		} else if (thread_state_sleeping(thread->state)) {
			sleepq_change_priority(thread->sleepq, thread, new_priority);
		} else {
			thread->sched_priority = new_priority;
		}

		assert(thread->sched_priority == new_priority);
	}
	sched_unlock();

	return 0;
}

void sched_set_priority(struct thread *thread,
		sched_priority_t new_priority) {
	assert((new_priority >= SCHED_PRIORITY_MIN)
			&& (new_priority <= SCHED_PRIORITY_MAX));

	sched_lock();
	{
		if (!thread_state_exited(thread->state)
				&& (thread != cpu_get_idle_thread())) {
			sched_change_scheduling_priority(thread, new_priority);
		}
		thread->initial_priority = new_priority;
	}
	sched_unlock();
}

static int switch_posted;

static void post_switch_if(int condition) {
	assert(in_sched_locked());

	if (condition) {
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
}

/**
 * Called by critical dispatching code with IRQs disabled.
 */
static void sched_switch(void) {
	struct thread *prev, *next;
	clock_t new_clock;

	assert(!in_sched_locked());

	sched_lock();
	{
		work_queue_run(&startq);

		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		ipl_enable();

		prev = sched_current();

		if (prev == (next = runq_switch(&rq))) {
			ipl_disable();
			goto out;
		}

		/* Running time recalculation */
		new_clock = clock();
		prev->running_time += new_clock - prev->last_sync;
		next->last_sync = new_clock;

		assert(thread_state_running(next->state));

		trace_point("context switch");

		ipl_disable();

		//task_notify_switch(prev, next);

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}

out:
	task_signal_hnd();
	sched_unlock_noswitch();
}

int sched_tryrun(struct thread *thread) {
	int res = 0;

	if (in_harder_critical()) {
		/* TODO: */
		//startq_enqueue_thread(thread, -EINTR);
		//critical_request_dispatch(&sched_critical);
	} else {
		sched_lock();
		{
			if (thread_state_sleeping(thread->state)) {
				sched_thread_notify(thread, -EINTR);
			} else if (!thread_state_running(thread->state)) {
				res = -1;
			}
		}
		sched_unlock();
	}

	return res;
}

int sched_cpu_init(struct thread *current) {
	extern int runq_cpu_init(struct runq *rq, struct thread *current);

	runq_cpu_init(&rq, current);

	current->last_sync = clock();
	thread_set_current(current);

	return 0;
}

static int unit_init(void) {
	work_queue_init(&startq);

	return 0;
}

static int unit_fini(void) {
	return 0;
}
