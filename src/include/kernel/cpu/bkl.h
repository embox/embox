/**
 * @file
 * @brief
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_CPU_BKL_H_
#define KERNEL_CPU_BKL_H_

#include <module/embox/kernel/cpu/bkl_api.h>

extern void bkl_lock(void);
extern void bkl_unlock(void);

/* critical_enter() has to acquire and raise the count under one interrupt
 * mask, so it needs the try and the wait apart. bkl_wait() spins on a load
 * rather than a compare-exchange, and keeps feeding the contention detector. */
extern int bkl_trylock(void);
extern void bkl_wait(void);
extern void bkl_assert_owned(unsigned int tested, unsigned int unit,
    void *from);

/* Does THIS cpu hold the lock? Asked without asserting, so that the interrupt
 * entry can count what it finds instead of dying of it. See bkl.c. */
extern int bkl_owned(void);

/* Raised by the architecture's interrupt entry. See bkl.c. */
extern unsigned long bkl_irq_nested;
extern unsigned long bkl_irq_unowned;
extern unsigned long bkl_irq_total;
extern unsigned long bkl_irq_zero;

#endif /* !KERNEL_CPU_BKL_H_ */
