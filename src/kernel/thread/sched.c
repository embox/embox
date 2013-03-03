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
#include <hal/context.h>
#include <hal/ipl.h>

#include <profiler/tracing/trace.h>

#include <time.h>

#include <embox/unit.h>

EMBOX_UNIT(unit_init, unit_fini);

/* Functions to work with waking up in interrupt. Implemented in startq.c */
extern void startq_flush(void);
extern void startq_enqueue_sleepq(struct sleepq *sq, int wake_all);
extern void startq_enqueue_thread(struct thread *t, int sleep_result);

void do_wake_thread(struct thread *thread, int sleep_result);
void do_wake_sleepq(struct sleepq *sq, int wake_all);

static void do_sleep_locked(struct sleepq *sq);

static void __sched_wake(struct sleepq *sq, int wake_all);

static void post_switch_if(int condition);

static void sched_switch(void);

void sched_post_switch(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

static struct runq rq;

static clock_t prev_clock;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

int sched_init(struct thread* current, struct thread *idle) {
	prev_clock = clock();

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

void sched_wake_all(struct sleepq *sq) {
	__sched_wake(sq, 1);
}

void sched_wake_one(struct sleepq *sq) {
	__sched_wake(sq, 0);
}

void __sched_wake(struct sleepq *sq, int wake_all) {
	if (in_harder_critical()) {
		startq_enqueue_sleepq(sq, wake_all);
		critical_request_dispatch(&sched_critical);
	} else {
		do_wake_sleepq(sq, wake_all);
	}
}

void do_wake_sleepq(struct sleepq *sq, int wake_all) {
	assert(!in_harder_critical());

	sched_lock();
	{
		post_switch_if(sleepq_wake(&rq, sq, wake_all));
	}
	sched_unlock();
}

void thread_wake_force(struct thread *thread, int sleep_result) {
	if (in_harder_critical()) {
		startq_enqueue_thread(thread, sleep_result);
		critical_request_dispatch(&sched_critical);
	} else {
		do_wake_thread(thread, sleep_result);
	}
}

void do_wake_thread(struct thread *thread, int sleep_result) {
	assert(!in_harder_critical());
	assert(thread_state_sleeping(thread->state));

	sched_lock();
	{
		thread->sleep_res = sleep_result;
		post_switch_if(sleepq_wake_thread(&rq, thread->sleepq, thread));
	}
	sched_unlock();
}

static void do_sleep_locked(struct sleepq *sq) {
	struct thread *current = sched_current();
	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	runq_sleep(&rq, sq);

	assert(current->sleepq == sq);
	assert(thread_state_sleeping(current->state));

	post_switch_if(1);
}

static void timeout_handler(struct sys_timer *timer, void *sleep_data) {
	struct thread *thread = (struct thread *) sleep_data;
	thread_wake_force(thread, -ETIMEDOUT);
}

int sched_sleep_locked_ns(struct sleepq *sq, unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = sched_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	current->sleep_res = ENOERR; /* clean out sleep_res */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init_ns(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	do_sleep_locked(sq);

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->sleep_res;
}

int sched_sleep_locked_us(struct sleepq *sq, unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = sched_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	current->sleep_res = ENOERR; /* clean out sleep_res */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init_us(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	do_sleep_locked(sq);

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->sleep_res;
}

int sched_sleep_locked_ms(struct sleepq *sq, unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = sched_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	current->sleep_res = ENOERR; /* clean out sleep_res */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init_ms(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	do_sleep_locked(sq);

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->sleep_res;
}

int sched_sleep_ns(struct sleepq *sq, unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_sleep_locked_ns(sq, timeout);
	}
	sched_unlock();

	return sleep_res;
}

int sched_sleep_us(struct sleepq *sq, unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_sleep_locked_us(sq, timeout);
	}
	sched_unlock();

	return sleep_res;
}

int sched_sleep_ms(struct sleepq *sq, unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_sleep_locked_ms(sq, timeout);
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
	if (thread == sched_current()) {
		/* Recalculate time of the current thread. */
		sched_lock();
		{
			clock_t	new_clock = clock();
			thread->running_time += new_clock - prev_clock;
			prev_clock = new_clock;
		}
		sched_unlock();
	}

	return thread->running_time;
}

int sched_change_scheduling_priority(struct thread *t,
		__thread_priority_t new_priority) {
	sched_lock();
	{
		assert(!thread_state_exited(t->state));

		if (thread_state_running(t->state)) {
			post_switch_if(runq_change_priority(t->runq, t, new_priority));
		} else if (thread_state_sleeping(t->state)) {
			sleepq_change_priority(t->sleepq, t, new_priority);
		} else {
			t->priority = new_priority;
		}

		assert(t->priority == new_priority);
	}
	sched_unlock();

	return 0;
}

void sched_set_priority(struct thread *t, __thread_priority_t new) {
	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			sched_change_scheduling_priority(t, new);
		}
		t->initial_priority = new;
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
		startq_flush();

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

		new_clock = clock();
		prev->running_time += new_clock - prev_clock;
		prev_clock = new_clock;

		assert(thread_state_running(next->state));

		trace_point("context switch");

		ipl_disable();

		task_notify_switch(prev, next);

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}

out:
	sched_unlock_noswitch();
}

int sched_tryrun(struct thread *thread) {
	int res = 0;

	assert(!in_harder_critical());

	sched_lock();

	{
		if (thread_state_sleeping(thread->state)) {
			do_wake_thread(thread, -EINTR);
		} else if (!thread_state_running(thread->state)) {
			res = -1;
		}
	}

	sched_unlock();

	return res;
}

int sched_cpu_init(struct thread *current) {
	extern int runq_cpu_init(struct runq *rq, struct thread *current);

	runq_cpu_init(&rq, current);

	thread_set_current(current);

	return 0;
}

static int unit_init(void) {
	return 0;
}

static int unit_fini(void) {
	return 0;
}
