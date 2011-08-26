/**
 * @file
 * @brief Kernel critical API.
 *
 * @details
 *
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   swirq_hnd   swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   30 ... 25   24 ... 19   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * For example, bit masks of the level corresponding to hardware interrupt
 * locks are the following:
 @verbatim
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 level    sched_lck   swirq_hnd   swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   30 ... 25   24 ... 19   17 ... 12   11 ...  6    5 ...  0
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
 bit_nr   30 ... 25   24 ... 19   17 ... 12   11 ...  6    5 ...  0
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 mask                  *  **  *
 harder                            *  **  *    *  **  *    *  **  *
 softer    *  **  *
 count                        *
 ------   -- --- --   -- --- --   -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_H_
#define KERNEL_CRITICAL_API_H_

#include <assert.h>
#include <stddef.h>

#include __impl_x(kernel/critical/critical_impl.h)

/* Critical levels mask. */

#define CRITICAL_IRQ_LOCK         0x0000003f
#define CRITICAL_IRQ_HANDLER      0x00000fc0
#define CRITICAL_SOFTIRQ_LOCK     0x0003f000
#define CRITICAL_SOFTIRQ_HANDLER  0x00fc0000
#define CRITICAL_SCHED_LOCK       0x3f000000

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

typedef __critical_t critical_t;

struct critical_dispatcher {
	struct critical_dispatcher *next;
	critical_t mask;
	void (*dispatch)(void);
};

#define CRITICAL_DISPATCHER_DEF(name, dispatch_fn, critical_mask) \
	static struct critical_dispatcher name = {       \
		.dispatch = (dispatch_fn),                   \
		.mask = ~((critical_mask)                    \
				| __CRITICAL_HARDER(critical_mask)), \
	}

static inline int critical_allows(critical_t level) {
	return !(__critical_count_get() & (level | __CRITICAL_HARDER(level)));
}

static inline int critical_inside(critical_t level) {
	return __critical_count_get() & level;
}

static inline void critical_enter(critical_t level) {
	__critical_count_add(__CRITICAL_COUNT(level));
}

static inline void critical_leave(critical_t level) {
	__critical_count_sub(__CRITICAL_COUNT(level));
}

static inline int critical_pending(struct critical_dispatcher *d) {
	return d->mask & 0x1;
}

extern void critical_request_dispatch(struct critical_dispatcher *d);
extern void critical_dispatch_pending(void);

/* Self-debugging stuff. */

/* Valid critical level mask is a single contiguous block of set bits. */
#define __CRITICAL_CHECK_BIT_BLOCK(level) \
	(__CRITICAL_SOFTER(level) | (level) | __CRITICAL_HARDER(level))

/* Check all level masks. */
#if ~0 != \
	  __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_LOCK)        \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_HANDLER)     \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_LOCK)    \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_HANDLER) \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_LOCK)
# error "CRITICAL_XXX must contain a single contiguous block of bits"
#endif

#endif /* KERNEL_CRITICAL_API_H_ */


