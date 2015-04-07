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
#include <sched.h>

#include <kernel/sched.h>

#include <hal/context.h>
#include <hal/cpu.h>
#include <hal/ipl.h>

#include <kernel/critical.h>
#include <kernel/spinlock.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/sched/current.h>

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

int sched_init(struct schedee *current) {

	runq_init(&rq.queue);
	rq.lock = SPIN_UNLOCKED;

	sched_set_current(current);

	return 0;
}

int schedee_init(struct schedee *schedee, int priority,
	struct schedee *(*process)(struct schedee *prev, struct schedee *next))
{
	runq_item_init(&schedee->runq_link);

	schedee->lock = SPIN_UNLOCKED;

	schedee->process = process;

	schedee->ready = false;
	schedee->active = false;
	schedee->waiting = true;

	schedee_priority_init(schedee, priority);
	sched_affinity_init(&schedee->affinity);
	sched_timing_init(schedee);

	return 0;
}

void sched_set_current(struct schedee *schedee) {
	assert(schedee_get_current() == NULL);
	__schedee_set_current(schedee);

	schedee->ready = true;
	schedee->active = true;
	schedee->waiting = false;
}

static void sched_check_preempt(struct schedee *t) {
	// TODO ask runq
	if (schedee_priority_get(schedee_get_current()) <
			schedee_priority_get(t))
		sched_post_switch(); // TODO SMP
}

/** Locks: IPL, thread, runq. */
static void __sched_enqueue(struct schedee *s) {
	runq_insert(&rq.queue, s);
}

/** Locks: IPL, thread, runq. */
static void __sched_dequeue(struct schedee *s) {
	runq_remove(&rq.queue, s);
}

/** Locks: IPL, thread, runq. */
static void __sched_enqueue_set_ready(struct schedee *s) {
	__sched_enqueue(s);
	s->ready = true;  /* let rq to see the previous state */
}

/** Locks: IPL, thread, runq. */
static void __sched_wokenup_clear_waiting(struct schedee *s) {
	sched_check_preempt(s);
	s->waiting = false;
}

int sched_active(struct schedee *s) {
	return s->active;
}

int sched_change_priority(struct schedee *s, int prior,
		int (*set_priority)(struct schedee_priority *, int)) {
	ipl_t ipl;
	int in_rq;

	assert(s);

	ipl = spin_lock_ipl(&rq.lock);
	in_rq = s->ready && !sched_active(s);

	if (in_rq)
		__sched_dequeue(s);
	set_priority(&s->priority, prior);
	if (in_rq)
		__sched_enqueue(s);

	sched_check_preempt(s);

	spin_unlock_ipl(&rq.lock, ipl);

	return 0;
}

static void __sched_freeze(struct schedee *s) {
	int in_rq;

	assert(s);

	spin_lock(&rq.lock);
	{
		in_rq = s->ready && !sched_active(s);

		if (in_rq)
			__sched_dequeue(s);

		s->ready = false;

		/* XXX ponder on safety of the below code outside of the rq->lock*/
		s->active = false;
		s->waiting = false;
	}
	spin_unlock(&rq.lock);
}

void sched_freeze(struct schedee *s) {
	assert(s);
	/* XXX acquired mostly for t->waiting */
	SPIN_IPL_PROTECTED_DO(&s->lock, __sched_freeze(s));
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
static int __sched_wakeup_ready(struct schedee *s) {
	int ready;

	/* This doesn't necessarily spin until the lock is acquired.
	 * SMP 'schedule' could outrun us getting the lock, but it will
	 * clear t->ready state as soon as possible thus letting us to go. */
	spin_protected_if (&rq.lock, (ready = s->ready))
		/* Event has arrived before the thread reached 'schedule' and
		 * went asleep (it could be even preempted after setting its
		 * t->waiting state).
		 * Just clear t->waiting state so that only a preemption check
		 * is done by the thread when it finally invokes the scheduler. */
		s->waiting = false;

	return ready;
}


/** Locks: IPL, thread. */
static void __sched_wakeup_waiting(struct schedee *s) {
	assert(s && s->waiting);

	spin_lock(&rq.lock);
	__sched_enqueue_set_ready(s);
	__sched_wokenup_clear_waiting(s);
	spin_unlock(&rq.lock);
}

#ifdef SMP

/** Locks: IPL, thread. */
static inline void __sched_wakeup_smp_inactive(struct schedee *s) {
	s->waiting = TW_SMP_WAKING;
	smp_membar();  /* __sched_smp_deactivate: ST active / LD waiting */
	if (!s->active)
		__sched_wakeup_waiting(s);
}

#else /* !SMP */

static inline void __sched_wakeup_smp_inactive(struct schedee *s) {
	/* The whole scheduler is IPL protected so in case of non-SMP kernel
	 * hitting these lines means that the target has already left 'schedule',
	 * i.e. it is inactive. No additional check is needed. */
	__sched_wakeup_waiting(s);
}

#endif /* SMP */


/** Called with IRQs off and thread lock held. */
int __sched_wakeup(struct schedee *s) {
	int was_waiting = (s->waiting && s->waiting != TW_SMP_WAKING);

	if (was_waiting)
		/* Check if t->ready state is still set, and we can do
		 * a fast-path wake up, that just clears t->waiting state.  */
		if (!__sched_wakeup_ready(s))
			/* Waiting but not ready: in case of SMP there is a slight chance
			 * that the target thread is still on a CPU (in the middle of
			 * 'schedule'). In such case the real wake up is performed on that
			 * CPU itself upon reaching the end of 'schedule'. */
			__sched_wakeup_smp_inactive(s);

	return was_waiting;
}

int sched_wakeup(struct schedee *s) {
	assert(s);
	return SPIN_IPL_PROTECTED_DO(&s->lock, __sched_wakeup(s));
}

/** Locks: IPL. */
static void __sched_activate(struct schedee *s) {
	s->active = true;
}

/** Locks: IPL. */
static void __sched_deactivate(struct schedee *s) {
	smp_stmembar();  /* don't clear active until context_switch is complete */
	s->active = false;
	smp_membar();  /* __sched_wakeup_smp_inactive: ST waiting / LD active */
#ifdef SMP
	spin_protected_if (&s->lock, (s->waiting == TW_SMP_WAKING))
		__sched_wakeup_waiting(s);
#endif /* SMP */
}

void sched_finish_switch(struct schedee *prev) {
	__sched_deactivate(prev);
}

void sched_start_switch(struct schedee *next) {
	__sched_activate(next);
}

static void __schedule(int preempt) {
	struct schedee *prev;
	struct schedee *next;

	prev = schedee_get_current();

	assert(!sched_in_interrupt());
	spin_lock_ipl(&rq.lock);

	if (!preempt && prev->waiting)
		prev->ready = false;
		/* In SMP kernel starting from this point and until clearing
		 * prev->active state (which is done by '__sched_deactivate')
		 * any CPU waking prev will move it to TW_SMP_WAKING state
		 * without really waking it up.
		 * 'sched_finish_switch' will sort out what to do in such case. */
	else
		__sched_enqueue(prev);

	sched_timing_stop(prev);

	while (1) {
		next = runq_extract(&rq.queue);

		/* Runq is unlocked as soon as possible, but interrupts remain disabled
		 * during the 'sched_switch' (if any). */
		spin_unlock(&rq.lock);

		/* next->process has to restore ipl. */
		next = next->process(prev, next);

		if (next) {
			break;
		}

		spin_lock_ipl(&rq.lock);
	}

	sched_timing_start(next);
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
	sched_unlock_noswitch();
}

void sched_wait_prepare(void) {
	struct schedee *s = schedee_get_current();

	// TODO SMP barrier? -- Eldar
	s->waiting = true;
}

void sched_wait_cleanup(void) {
	struct schedee *s = schedee_get_current();

	s->waiting = false;
	// TODO SMP barrier? -- Eldar
}
