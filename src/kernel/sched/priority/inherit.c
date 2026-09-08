/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#include <kernel/sched/schedee_priority.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched.h>
#include <errno.h>

int schedee_priority_init(struct schedee *s, int new_priority) {
	s->priority.base_priority = new_priority;
	s->priority.current_priority = new_priority;

	return 0;
}

/* Taking sched_lock() here is a lock-order inversion, and it deadlocks the
 * whole machine.
 *
 * This runs as the callback of sched_change_priority(), which holds rq.lock
 * with interrupts off for the whole call. sched_lock() is
 * critical_enter(CRITICAL_SCHED_LOCK), and on the 0 -> level transition that
 * SPINS FOR THE BKL -- with interrupts still off, because the ipl it restores
 * around bkl_wait() is the one spin_lock_ipl(&rq.lock) already saved. So this
 * path takes rq.lock and then the BKL.
 *
 * __schedule() takes them the other way round: it is reached from
 * sched_unlock() -> critical_dispatch_pending() -> sched_preempt() with the
 * BKL held, and its first act is spin_lock_ipl(&rq.lock). Two cores, one in
 * each path, and neither can ever finish -- both with interrupts disabled, so
 * no core takes another timer interrupt and the entire system stops.
 *
 * rq.lock is the stronger lock and it is already held: the two sibling
 * callbacks, __schedee_priority_inherit and __schedee_priority_reverse, write
 * the same fields under it and nothing else. This one now does the same. */
static int __schedee_priority_set(struct schedee_priority *p, int new_priority) {
	if (p->base_priority == new_priority) {
		return 0;
	}

	/* If we work with scheduling thread we must take into consideration
	 * that priority can be changed due to inheritance
	 */
	if (p->base_priority == p->current_priority) {
		p->base_priority = new_priority;
		p->current_priority = new_priority;
	}
	else {
		p->base_priority = new_priority;
		if (p->current_priority <= new_priority) {
			p->current_priority = new_priority;
		}
	}

	return 0;
}

int schedee_priority_set(struct schedee *s, int new_priority) {
	if ((new_priority < SCHED_PRIORITY_MIN)
			|| (new_priority > SCHED_PRIORITY_MAX)) {
		return -EINVAL;
	}

	return sched_change_priority(s, new_priority, &__schedee_priority_set);
}

int schedee_priority_get(struct schedee *s) {
	return s->priority.current_priority;
}

/* A priority is not a number the schedee carries, it is the queue it is filed
 * in.
 *
 * These two used to assign to current_priority and stop there. For a schedee
 * that is RUNNING that is enough -- the scheduler reads the number when it
 * next files it -- and that is the case the tree's own inheritance test
 * covered, which is why this survived. For a schedee that is QUEUED it does
 * nothing at all: the schedee stays in the old level's list and the scheduler
 * goes on picking it by the old priority.
 *
 * That is exactly the case inheritance exists for. A mutex holder preempted
 * with the mutex in its hands cannot give it back until it runs, and it will
 * not run until it is boosted -- so the boost has to move it, not describe it.
 *
 * sched_change_priority() already does the whole thing under rq.lock: dequeue
 * if queued, set, enqueue, and ask for a preemption if the result outranks
 * whoever is running. */
static int __schedee_priority_inherit(struct schedee_priority *p, int prior) {
	if (prior > p->current_priority) {
		p->current_priority = prior;
	}

	return 0;
}

int schedee_priority_inherit(struct schedee *s, int priority) {
	sched_change_priority(s, priority, &__schedee_priority_inherit);

	return s->priority.current_priority;
}

static int __schedee_priority_reverse(struct schedee_priority *p, int prior) {
	(void)prior;

	p->current_priority = p->base_priority;

	return 0;
}

int schedee_priority_reverse(struct schedee *s) {
	/* The mirror of the above: giving a boost back is also a move between
	 * levels, and whoever was waiting behind the boosted schedee has to get
	 * the CPU when it stops outranking them. */
	sched_change_priority(s, s->priority.base_priority,
	    &__schedee_priority_reverse);

	return s->priority.current_priority;
}
