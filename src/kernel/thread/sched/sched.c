/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Skorodumov Kirill
 */

#include <errno.h>
#include <lib/list.h>
#include <kernel/scheduler.h>
#include <kernel/scheduler_base.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(scheduler_init);

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

/**
 * If it doesn't equal to zero,
 * we are located in critical section
 * and can't switch between threads.
 */
static int preemption_count = 1;

/**
 * Initializes scheduler.
 */
static int scheduler_init(void) {
	_scheduler_init();
//	scheduler_current()->reschedule = false;
	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void scheduler_tick(uint32_t id) {
	LOG_DEBUG("\nTick\n");
	scheduler_current()->reschedule = true;
}

void scheduler_start(void) {
	LOG_DEBUG("\nStart scheduler\n");
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, scheduler_tick);
	idle_thread->reschedule = true;
	_scheduler_start();
	scheduler_unlock();
	LOG_DEBUG("\nPreemtion count = %d", preemption_count);
	LOG_DEBUG("\nCurrent thread reschedule = %d\n", thread_current()->reschedule);
}

void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	preemption_count--;
	if (preemption_count == 0 && scheduler_current()->reschedule) {
		scheduler_dispatch();
	}
}

static void preemption_inc(void) {
	preemption_count++;
}

void scheduler_dispatch(void) {
	ipl_t ipl;
	struct thread *prev_thread;
	struct thread *next_thread;


	if (preemption_count == 0 && scheduler_current()->reschedule) {
		preemption_inc();
		prev_thread = scheduler_current();
		next_thread = _scheduler_next(prev_thread);
		prev_thread->reschedule = false;

		#ifdef CONFIG_PP_ENABLE
		if (thread_current()->pp != prev_thread->pp) {
			pp_store( prev_thread->pp );
			pp_restore( thread_current()->pp );
		}
		#endif

		LOG_DEBUG("\nSwitching from %d to %d\n",
			prev_thread->id, thread_current()->id);
		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &next_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
	scheduler_lock();
	_scheduler_add(added_thread);
	scheduler_unlock();
}

void scheduler_stop(void) {
	scheduler_lock();
	LOG_DEBUG("\nStop scheduler\n");
	close_timer (THREADS_TIMER_ID);
	_scheduler_stop();
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	_scheduler_remove(removed_thread);
	scheduler_unlock();
	return 0;
}

int scheduler_sleep(struct event *event) {
	scheduler_lock();
	scheduler_current()->state = THREAD_STATE_WAIT;
	list_add(&scheduler_current()->wait_list, &event->threads_list);
	scheduler_remove(scheduler_current());
	LOG_DEBUG("\nLocking %d\n", thread_current()->id);
	scheduler_unlock();
	return 0;
}

int scheduler_wakeup(struct event *event) {
	struct thread *thread;
	struct thread *tmp_thread;
	scheduler_lock();
	list_for_each_entry_safe(thread, tmp_thread,
			&event->threads_list, wait_list) {
		list_del_init(&thread->wait_list);
		thread->state = THREAD_STATE_RUN;
		scheduler_add(thread);
		LOG_DEBUG("\nUnlocking %d\n", thread->id);
	}
	scheduler_unlock();
	return 0;
}

int scheduler_wakeup_first(struct event *event) {
	struct thread *thread;
	scheduler_lock();
	thread = list_entry(event->threads_list.next, struct thread, wait_list);
	list_del_init(&thread->wait_list);
	thread->state = THREAD_STATE_RUN;
	scheduler_add(thread);
	LOG_DEBUG("\nUnlocking %d\n", thread->id);
	scheduler_unlock();
	return 0;
}

struct thread *scheduler_current(void){
	return _scheduler_current();
}

void event_init(struct event *event) {
	INIT_LIST_HEAD(&event->threads_list);
}
