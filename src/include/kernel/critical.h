/**
 * Kernel critical API.
 *
 * Embox kernel is fully preemptive and supports both hardware and software
 * interrupts. Preemption in terms of Critical API has more generic meaning
 * than a regular preemption of a thread by a one with higher priority and
 * also includes hardware and software interrupts. In other words, we speak
 * about preemption of execution contexts, not only threads.
 *
 * Lets draw up a quick overview of possible execution contexts:
 *
 *   - Non-preemptable context between calls of #ipl_save() and #ipl_restore()
 *     in which no hardware interrupt can occur and which must be as fast as
 *     possible to retain suitable reaction time of the system. Of course there
 *     can't be any software interrupt dispatched or thread switched within
 *     such sections.
 *
 *   - Hardware IRQ handlers which can be entered at any time except when
 *     certain interrupts are masked on the CPU using IPL. An IRQ with
 *     higher priority can also preempt others, thus handlers can be nested.
 *     As in the previous described context, soft interrupts and scheduling are
 *     disabled within hardware interrupt handlers.
 *
 *   - Software interrupts are designed by analogy with the hardware
 *     interrupts. Software interrupts can be locked too to temporally prevent
 *     dispatching and deferring it when they become unlocked again.
 *
 *   - Software IRQ handlers that can nest similarly to hardware ones.
 *     Because a handler can start execution only outside hardware IRQ context
 *     it can be preempted as well. However scheduling from the handler is
 *     still impossible.
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
 * it becomes rather complicated to answer questions like:
 *   - If driver raises a software interrupt being inside a hardware interrupt
 *     handler, so when the kernel have to actually invoke it?
 *     When leaving the hardware interrupt handler? Or leaving the outermost
 *     handler in case of interrupts nesting? And what if someone have
 *     locked software interrupts previously?
 *
 *   - How should we check for pending soft interrupt dispatching? Is such
 *     check is needed on each unlock and after leaving hardware interrupt
 *     handler? And how to be in case when the system is configured without
 *     softirq support at all?
 *
 *   - What's about reentrancy and interrupt safety?
 *
 * Critical API consolidates tracking of the current execution context and
 * provides a way to give answers for such questions.
 *
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   swirq_hnd   swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   29 ... 24   23 ... 18   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * For example, bit masks of the level corresponding to hardware interrupt
 * locks are the following:
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   swirq_hnd   swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   29 ... 24   23 ... 18   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 mask                                                      *  **  *
 harder
 softer    *  **  *    *  **  *    *  **  *    *  **  *
 count                                                            *
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * And masks being used when entering/leaving software interrupt handlers look
 * like:
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   swirq_hnd   swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   29 ... 24   23 ... 18   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 mask                  *  **  *
 harder                            *  **  *    *  **  *    *  **  *
 softer    *  **  *
 count                        *
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
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
#define CRITICAL_SCHED_LOCK       0x0003f000 /**< 64 calls. */

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

static inline int critical_allows(unsigned int level) {
	return !(critical_count() & (level | __CRITICAL_HARDER(level)));
}

static inline int critical_inside(unsigned int level) {
	return critical_count() & level;
}

static inline void critical_enter(unsigned int level) {
	__critical_count_add(__CRITICAL_COUNT(level));
	if (critical_count() == __CRITICAL_COUNT(level))
		bkl_lock();
}

static inline void critical_leave(unsigned int level) {
	if (critical_count() == __CRITICAL_COUNT(level))
		bkl_unlock();
	__critical_count_sub(__CRITICAL_COUNT(level));
}

static inline int critical_pending(struct critical_dispatcher *d) {
	return d->mask & 0x1;
}


#include <sys/cdefs.h>
__BEGIN_DECLS

extern void critical_request_dispatch(struct critical_dispatcher *d);
extern void critical_dispatch_pending(void);

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
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_LOCK)
# error "CRITICAL_XXX must contain a single contiguous block of bits"
#endif

#endif /* KERNEL_CRITICAL_H_ */
