/**
 * Kernel critical API.
 *
 * Embox kernel is fully preemptive and supports hardware interrupts and
 * differed interrupt processing via lthreads. Preemption in terms of
 * Critical API has more generic meaning than regular thread preemption and
 * also includes hardware interrupts. In other words, we speak about preemption
 * of execution contexts, not only threads.
 *
 * Lets draw up a quick overview of possible execution contexts:
 *
 *   - A non-preemptable context between calls of #ipl_save() and #ipl_restore()
 *     in which no hardware interrupt can occur and which must be as fast as
 *     possible to retain suitable reaction time of the system. Of course
 *     neither threads nor light threads can be scheduled in this context.
 *
 *   - Hardware IRQ handlers which can be entered at any time except when
 *     certain interrupts are masked on the CPU using IPL. An IRQ with
 *     higher priority can also preempt others, thus handlers can be nested.
 *     As in the previously described context, scheduling is disabled within
 *     hardware interrupt handlers.
 *
 *   - And finally there is a special context for scheduling locked state to
 *     protect internal structures of the scheduler itself.
 *
 * As you may have noticed, there is a one regularity in the list above.
 * A context can be dispatched (run a specific handler) only being outside
 * any context which is more critical than the given one. Otherwise the
 * dispatching is deferred until the execution leaves these contexts.
 *
 * @code
 *  irq_lock(); // Kernel enters interrupts locked context.
 *  {
 *  	...
 *  	// A hardware interrupt occurs here but actual dispatching is deferred.
 *  	...
 *  }
 *  irq_unlock(); // Interrupts are unlocked and pending dispatch takes place.
 *
 *  irq_dispatch(); // Processing pending interrupt requests.
 *  {
 *  	some_irq_handler(); // Registered interrupt handler is invoked.
 *  	{
 *  		...
 *  		// Handler wakes up some thread,
 *  		// but rescheduling can't occur immediately
 *  		// because of being inside interrupt handler.
 *  		sched_wakeup();
 *  		...
 *  	}
 *  }
 *
 *  scheduling(); // Scheduling and thread preemption is performed here.
 *
 * @endcode
 *
 * It becomes rather complicated to answer questions like:
 *   - If some driver wakes lthread being inside a hardware interrupt handler,
 *     so when the kernel have to actually invoke it? When leaving the hardware
 *     interrupt handler? Or leaving the outermost handler in case of interrupts
 *     nesting? And what if someone have locked software interrupts previously?
 *
 *   - What's about reentrancy and interrupt safety?
 *
 * Critical API consolidates tracking of the current execution context and
 * provides a way to give answers for such questions.
 *
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   preempt     hwirq_hnd   hwirq_lck
 bit_nr   23 ... 18   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * For example, bit masks of the level corresponding to hardware interrupt
 * locks are the following:
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   preempt     hwirq_hnd   hwirq_lck
 bit_nr   23 ... 18   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --
 mask                                          *  **  *
 harder
 softer    *  **  *    *  **  *    *  **  *
 count                                                *
 ------   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * The preempt block is new: the half of the old sched_lck block that
 * __spin_preempt_disable() used to borrow. It sits below sched_lck so a
 * pending sched_preempt() stays deferred while a spin region is open, and is
 * excluded from __CRITICAL_BKL_MASK so opening one does not claim the BKL.
 *
 * @file
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
#define KERNEL_CRITICAL_H_

/* Critical levels mask. */

#define CRITICAL_IRQ_LOCK         0x0000003f /**< 64 calls depth. */
#define CRITICAL_IRQ_HANDLER      0x00000fc0 /**< 64 nested interrupts. */
#define CRITICAL_PREEMPT_LOCK     0x0003f000 /**< 64 spin regions. */
#define CRITICAL_SCHED_LOCK       0x00fc0000 /**< 64 calls. */

/* The part of the count that means "this CPU holds the BKL". Preemption is
 * disabled without it, so its bits do not answer for the lock. */
#define __CRITICAL_BKL_MASK       (~CRITICAL_PREEMPT_LOCK)

/* Internal helper macros for bit masks transformation. */

/* 01111000 *
 * 01110111 *
 * 00001111 *
 * 00000111 */
#define __CRITICAL_HARDER(level) \
	(((level) ^ ((level) - 1)) >> 1)

/* 01111000 *
 * 00000111 *
 * 00001000 */
#define __CRITICAL_COUNT(level) \
	(__CRITICAL_HARDER(level) + 1)

/* 01111000 *
 * 10000111 *
 * 00000111 *
 * 10000000 */
#define __CRITICAL_SOFTER(level) \
	(~(level) ^ __CRITICAL_HARDER(level))

#ifndef __ASSEMBLER__

#include <linux/compiler.h>
#include <hal/ipl.h>
#include <kernel/cpu/bkl.h>
#include <kernel/cpu/cpudata.h>

struct critical_dispatcher {
	struct critical_dispatcher *next;
	unsigned int mask; /**< Inverted in case when dispatching is not pending. */
	void (*dispatch)(void);
};

#define CRITICAL_DISPATCHER_DEF(name, dispatch_fn, critical_mask) \
	static struct critical_dispatcher name __cpudata__ = {        \
		.dispatch = (dispatch_fn),                                \
		.mask = ~((critical_mask)                                 \
				| __CRITICAL_HARDER(critical_mask)),              \
	}

extern unsigned int __critical_count __cpudata__;

/** Local CPU counter, lvalue. */
#define critical_count() \
	cpudata_var(__critical_count)

static inline void __critical_count_add(unsigned long count) {
	critical_count() += count;
	__barrier();
}

static inline void __critical_count_sub(unsigned long count) {
	__barrier();
	critical_count() -= count;
}

/* The count is per-CPU and the caller may be preemptible -- a zero count is
 * exactly what these two are asked to confirm. Unmasked, cpudata_var() can
 * land on the core the thread just left. */
static inline int critical_allows(unsigned int level) {
	unsigned int cur;
	ipl_t ipl;

	ipl = ipl_save();
	cur = critical_count();
	ipl_restore(ipl);

	return !(cur & (level | __CRITICAL_HARDER(level)));
}

static inline int critical_inside(unsigned int level) {
	unsigned int cur;
	ipl_t ipl;

	ipl = ipl_save();
	cur = critical_count();
	ipl_restore(ipl);

	return cur & level;
}

/* Raising the count and taking the lock must look like one step to this CPU's
 * own interrupts, or a handler that lands between them reads a count that
 * promises a lock nobody holds. Only the outermost entry pays for the mask,
 * and only across the acquisition -- the wait runs at the caller's interrupt
 * level, with the count still zero, which a handler reads correctly. */
static inline void critical_enter(unsigned int level) {
	unsigned int count = __CRITICAL_COUNT(level);
	unsigned int cur;
	ipl_t ipl;

	/* cpudata_var() is `ask which CPU, then address that CPU's copy`; a
	 * zero-count thread can be preempted between the two, onto another CPU.
	 * Masking interrupts is the fix: the handler is the only way into the
	 * scheduler from here. */
	ipl = ipl_save();
	cur = critical_count();

	if (cur & __CRITICAL_BKL_MASK) {
		/* Nested, so this CPU holds the lock and cannot be preempted out of
		 * it. The mask above makes the read that said so this CPU's own; the
		 * count needs no protection once non-zero. */
		bkl_assert_owned(cur, count, __builtin_return_address(0));
		__critical_count_add(count);
		ipl_restore(ipl);
		return;
	}

	while (!bkl_trylock()) {
		ipl_restore(ipl);
		bkl_wait();
		ipl = ipl_save();
	}
	__critical_count_add(count);
	ipl_restore(ipl);
}

static inline void critical_leave(unsigned int level) {
	unsigned int count = __CRITICAL_COUNT(level);
	unsigned int cur = critical_count();
	ipl_t ipl;

	if ((cur & __CRITICAL_BKL_MASK) != count) {
		bkl_assert_owned(cur, count, __builtin_return_address(0));
		__critical_count_sub(count);
		return;
	}

	ipl = ipl_save();
	__critical_count_sub(count);
	bkl_unlock();
	ipl_restore(ipl);
}

static inline int critical_pending(struct critical_dispatcher *d) {
	return d->mask & 0x1;
}


#include <sys/cdefs.h>
__BEGIN_DECLS

extern void critical_request_dispatch(struct critical_dispatcher *d);
extern void critical_dispatch_pending(void);
extern int  critical_dispatch_required(void);

__END_DECLS

#endif /* __ASSEMBLER__ */

/* Self-debugging stuff. */

/* Valid critical level mask is a single contiguous block of set bits. */
#define __CRITICAL_CHECK_BIT_BLOCK(level) \
	(__CRITICAL_SOFTER(level) | (level) | __CRITICAL_HARDER(level))

/* Check all level masks. */
#if ~0 != \
	  __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_LOCK)        \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_HANDLER)     \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_PREEMPT_LOCK)    \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_LOCK)
# error "CRITICAL_XXX must contain a single contiguous block of bits"
#endif

#endif /* KERNEL_CRITICAL_H_ */
