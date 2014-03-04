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
 *          - SMP friendliness and lock-less wait logic
 */

#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <sched.h>

#include <kernel/sched.h>

#include <hal/context.h>
#include <hal/cpu.h>
#include <hal/ipl.h>

#include <kernel/critical.h>
#include <kernel/spinlock.h>
#include <kernel/sched/sched_timing.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread.h>

#include <kernel/runnable/runnable.h>
#include <kernel/lwthread.h>
#include <kernel/thread/current.h>
#include <kernel/thread/signal.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>

// XXX
#ifndef __barrier
#define __barrier() __asm__ __volatile__("" : : : "memory")
#endif

// XXX
#define smp_stmembar() __barrier()
#define smp_ldmembar() __barrier()
#define smp_membar()   __barrier()

static void sched_preempt(void);
CRITICAL_DISPATCHER_DEF(sched_critical, sched_preempt, CRITICAL_SCHED_LOCK);

//TODO these variable for scheduler (may be create object scheduler?)
static struct runq rq;

void sched_post_switch(void) {
	critical_request_dispatch(&sched_critical);
}

static inline int sched_in_interrupt(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

int sched_init(struct thread *idle, struct thread *current) {
	assert(idle && current);

	assert(thread_self() == current);

	runq_init(&rq.queue);
	rq.lock = SPIN_UNLOCKED;

	assert(idle->waiting); // XXX
	sched_wakeup(idle);

	sched_ticker_init();

	return 0;
}

static void sched_check_preempt(struct thread *t) {
	// TODO ask runq
	if (thread_priority_get(thread_self()) < thread_priority_get(t))
		sched_post_switch(); // TODO SMP
}

/** Locks: IPL, thread, runq. */
static void __sched_enqueue(struct runnable *r) {
	runq_insert(&rq.queue, r);
}

/** Locks: IPL, thread, runq. */
static void __sched_dequeue(struct thread *t) {
	runq_remove(&rq.queue, &(t->runnable));
}

/** Locks: IPL, thread, runq. */
static void __sched_enqueue_set_ready(struct thread *t) {
	__sched_enqueue(&(t->runnable));
	t->ready = true;  /* let rq to see the previous state */
}

/** Locks: IPL, thread, runq. */
static void __sched_wokenup_clear_waiting(struct thread *t) {
	sched_check_preempt(t);
	t->waiting = false;
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	ipl_t ipl;
	int in_rq;

	assert(t);

	ipl = spin_lock_ipl(&rq.lock);
	in_rq = t->ready && !sched_active(t);

	if (in_rq)
		__sched_dequeue(t);
	thread_priority_set(t, prior);
	if (in_rq)
		__sched_enqueue(&(t->runnable));

	sched_check_preempt(t);

	spin_unlock_ipl(&rq.lock, ipl);

	return 0;
}

static void __sched_freeze(struct thread *t) {
	int in_rq;

	assert(t);

	spin_lock(&rq.lock);
	{
		in_rq = t->ready && !sched_active(t);

		if (in_rq)
			__sched_dequeue(t);

		t->ready = false;

		/* XXX ponder on safety of the below code outside of the rq->lock*/
		t->active = false;
		t->waiting = false;
	}
	spin_unlock(&rq.lock);
}

void sched_freeze(struct thread *t) {
	assert(t);
	/* XXX acquired mostly for t->waiting */
	SPIN_IPL_PROTECTED_DO(&t->lock, __sched_freeze(t));
}

/*
 * Managing waiting state of a thread
 *
 * Basically there are two different cases of wait-wake interoperation plus
 * one additional in case of SMP.
 *
 *
 * The most simple case is when an event interruption occurs before the thread
 * enters the scheduler and disables interrupts. In this case IRQ handler
 * restores an initial state of the thread just by clearing t->waiting state.
 *
 * control flow               state      locks     IRQ
 * ------------------------   --------   --------  --------------------------
 *                            A R
 * self->waiting = 1          A R W
 * if (cond) ...              A R W
 *                            A R W      IPL    t  __s_wakeup_ready()
 *                            A R W      IPL rq t    lock(rq) if t->ready
 *                            A R        IPL rq t    t->waiting = 0
 *                            A R        IPL    t    unlock(rq)
 *                            A R
 * schedule() -------------   A R        IPL
 *   lock(rq)                 A R        IPL rq
 *   if (prev != next) ...    A R        IPL rq
 *   unlock(rq)               A R        IPL
 *                            A R
 *
 *
 * If an event arrives after the thread was scheduled out of the CPU, it
 * makes the thread t->ready, enqueues it, checks if it can preempt a thread
 * currently running and clears t->waiting. If the current thread needs to be
 * preempted, sched_preempt() will be called upon leaving all interrupt
 * handlers and the outermost sched_lock.
 *
 * control flow               state      locks     IRQ
 * ------------------------   --------   --------  --------------------------
 *                            A R
 * self->waiting = 1          A R W
 * if (cond) ...              A R W
 * schedule() -------------   A R W      IPL
 *   lock(rq)                 A R W      IPL rq
 *   prev->ready = 0          A   W      IPL rq
 *   unlock(rq)               A   W      IPL
 *   s_switch() -----------   A   W      IPL
 * ----- next thread ------   A   W      IPL
 *     prev->active = 0           W      IPL
 *                                W
 *                                W      IPL    t  __s_wakeup_waiting()
 *                                W      IPL rq t    lock(rq)
 *                              R W      IPL rq t    t->ready = 1
 *                              R        IPL rq t    t->waiting = 0
 *                              R        IPL    t    unlock(rq)
 *                              R
 *
 *
 * SMP add one another tricky corner case. If an event occurs on a remote CPU
 * while the current one performs 'sched_switch' (as prev) the actual enqueuing
 * must be delayed until the thread leaves the scheduler. Otherwise it could
 * be moved to an idle CPU, start 'sched_switch' (as next) thus trashing
 * context of the thread (saved registers state).
 *
 * To overcome this issue an intermediate TW_SMP_WAKING state is introduced
 * which is checked upon returning from 'sched_switch'.
 *
 * control flow               state      locks     SMP remote CPU
 * ------------------------   --------   --------  --------------------------
 *                            A R
 * self->waiting = 1          A R W
 * if (cond) ...              A R W
 * schedule() -------------   A R W      IPL
 *   lock(rq)                 A R W      IPL rq
 *                            A R W      IPL rq t   __s_wakeup_ready()
 *                            A R W      IPL rq t     lock(rq) if t->ready
 *   prev->ready = 0          A   W      IPL rq t
 *                            A   W      IPL rq t   __s_wakeup_smp_inactive()
 *   unlock(rq)               A   W      IPL    t
 *                            A   Wake   IPL    t     t->waiting = TW_SMP_WAKING
 *   s_switch() -----------   A   Wake   IPL
 * ----- next thread ------   A   Wake   IPL
 *     prev->active = 0           Wake   IPL
 *     __s_wakeup_waiting()       Wake   IPL    t
 *     lock(rq)                   Wake   IPL rq t
 *     prev->ready = 1          R Wake   IPL rq t
 *     prev->waiting = 0        R        IPL rq t
 *     unlock(rq)                        IPL    t
 */

/** Locks: IPL, thread. */
static int __sched_wakeup_ready(struct thread *t) {
	int ready;

	/* This doesn't necessarily spin until the lock is acquired.
	 * SMP 'schedule' could outrun us getting the lock, but it will
	 * clear t->ready state as soon as possible thus letting us to go. */
	spin_protected_if (&rq.lock, (ready = t->ready))
		/* Event has arrived before the thread reached 'schedule' and
		 * went asleep (it could be even preempted after setting its
		 * t->waiting state).
		 * Just clear t->waiting state so that only a preemption check
		 * is done by the thread when it finally invokes the scheduler. */
		t->waiting = false;

	return ready;
}


/** Locks: IPL, thread. */
static void __sched_wakeup_waiting(struct thread *t) {
	assert(t && t->waiting);

	spin_lock(&rq.lock);
	__sched_enqueue_set_ready(t);
	__sched_wokenup_clear_waiting(t);
	spin_unlock(&rq.lock);
}

#ifdef SMP

/** Locks: IPL, thread. */
static inline void __sched_wakeup_smp_inactive(struct thread *t) {
	t->waiting = TW_SMP_WAKING;
	smp_membar();  /* __sched_smp_deactivate: ST active / LD waiting */
	if (!t->active)
		__sched_wakeup_waiting(t);
}

#else /* !SMP */

static inline void __sched_wakeup_smp_inactive(struct thread *t) {
	/* The whole scheduler is IPL protected so in case of non-SMP kernel
	 * hitting these lines means that the target has already left 'schedule',
	 * i.e. it is inactive. No additional check is needed. */
	__sched_wakeup_waiting(t);
}

#endif /* SMP */


/** Called with IRQs off and thread lock held. */
int __sched_wakeup(struct thread *t) {
	int was_waiting = (t->waiting && t->waiting != TW_SMP_WAKING);

	if (was_waiting)
		/* Check if t->ready state is still set, and we can do
		 * a fast-path wake up, that just clears t->waiting state.  */
		if (!__sched_wakeup_ready(t))
			/* Waiting but not ready: in case of SMP there is a slight chance
			 * that the target thread is still on a CPU (in the middle of
			 * 'schedule'). In such case the real wake up is performed on that
			 * CPU itself upon reaching the end of 'schedule'. */
			__sched_wakeup_smp_inactive(t);

	return was_waiting;
}

int sched_wakeup(struct thread *t) {
	assert(t);
	return SPIN_IPL_PROTECTED_DO(&t->lock, __sched_wakeup(t));
}

void sched_lwthread_wake(struct lwthread *lwt) {
	__sched_enqueue(&(lwt->runnable));
}


/** Locks: IPL. */
static void __sched_activate(struct thread *t) {
	t->active = true;
}

/** Locks: IPL. */
static void __sched_deactivate(struct thread *t) {
	smp_stmembar();  /* don't clear active until context_switch is complete */
	t->active = false;
	smp_membar();  /* __sched_wakeup_smp_inactive: ST waiting / LD active */
#ifdef SMP
	spin_protected_if (&t->lock, (t->waiting == TW_SMP_WAKING))
		__sched_wakeup_waiting(t);
#endif /* SMP */
}

static void sched_prepare_switch(struct thread *prev, struct thread *next) {
	sched_ticker_switch(prev, next);
	sched_timing_switch(prev, next);
	prev->critical_count = critical_count();
	critical_count() = next->critical_count;
	__sched_activate(next);
}

static void sched_finish_switch(struct thread *prev) {
	__sched_deactivate(cpudata_var(prev));
}

static struct thread *saved_prev __cpudata__; // XXX

/**
 * Any fresh thread must call this function from a trampoline.
 * Basically it emulates returning from the scheduler as it would be done
 * in case if 'context_switch' would return as usual (into 'sched_switch',
 * from where it was called) instead of jumping into a thread trampoline.
 */

void sched_ack_switched(void) {
	sched_finish_switch(cpudata_var(saved_prev));
	ipl_enable();
	sched_unlock();
}

static void sched_switch(struct thread *prev, struct thread *next) {
	sched_prepare_switch(prev, next);

	trace_point(__func__);

	/* Preserve initial semantics of prev/next. */
	cpudata_var(saved_prev) = prev;
	thread_set_current(next);
	context_switch(&prev->context, &next->context);  /* implies cc barrier */
	prev = cpudata_var(saved_prev);

	sched_finish_switch(prev);
}

void sched_thread_switch(struct thread *prev, struct thread *next) {
	sched_switch(prev, next);
}

static void __schedule(int preempt) {
	struct thread *prev;
	struct runnable *next;
	ipl_t ipl;

	prev = thread_self();

	assert(!sched_in_interrupt());
	ipl = spin_lock_ipl(&rq.lock);

	if (!preempt && prev->waiting)
		prev->ready = false;
		/* In SMP kernel starting from this point and until clearing
		 * prev->active state (which is done by '__sched_deactivate')
		 * any CPU waking prev will move it to TW_SMP_WAKING state
		 * without really waking it up.
		 * 'sched_finish_switch' will sort out what to do in such case. */
	else
		__sched_enqueue(&(prev->runnable));

	next = runq_extract(&rq.queue);

	/* Runq is unlocked as soon as possible, but interrupts remain disabled
	 * during the 'sched_switch' (if any). */
	spin_unlock(&rq.lock);

	if (&(prev->runnable) != next) {
		if(next->prepare != NULL) {
			next->prepare(prev, next);
		}
	}

	ipl_restore(ipl);

	assert(thread_self() == prev);

	/* start for lwthreads */
	if(next->run != NULL) {
		next->run();
	}

	if (!prev->siglock) {
		thread_signal_handle();
	}
}

void schedule(void) {
	sched_lock();
	__schedule(0);
	sched_unlock();
}

/** Called by critical dispatching code with IRQs disabled. */
static void sched_preempt(void) {
	sched_lock();
	__schedule(1);
	sched_unlock();
}
