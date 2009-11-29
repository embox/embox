/**
 * @file irqlevel.h
 *
 * @brief TODO
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_IRQLEVEL_H_
#define HAL_IRQLEVEL_H_

#include <asm/haldefs.h>

/**
 * Linux-like wrapper macro for interrupts masking.
 * Denies any interrupt (except NMI) to occur saving necessary flags
 * to the variable named as the specified macro argument.
 *
 * @param flags the @c irqlevel_t typed variable to save flags into
 */
#define local_irq_save(flags)    ((level) = irqlevel_high())

/**
 * Linux-like wrapper macro for interrupts masking.
 * Restores the previous state indicated in the specified variable.
 *
 * @param flags the value returned by local_irq_save() macro
 */
#define local_irq_restore(flags) irqlevel_set(level)

/**
 * The implementation-dependent type suitable to hold IRQ level flags.
 */
typedef __irqlevel_t irqlevel_t;

/**
 * Sets the maximal interrupt masking level cutting off all IRQs on the CPU
 * (except non-maskable interrupts, if any).
 *
 * @return register value containing previous processor interrupt level
 */
irqlevel_t irqlevel_high();

/**
 * Sets the lowest interrupt masking level allowing all IRQs.
 *
 * @return register value containing previous processor interrupt level
 */
irqlevel_t irqlevel_low();

/**
 * Restores the previous interrupt level specified by the @c level argument.
 *
 * @param level the value returned by the previous invocation of this method
 *              or one of its derivatives
 * @return register value containing previous processor interrupt level
 *
 * @sa irqlevel_high()
 */
irqlevel_t irqlevel_set(irqlevel_t level);

#endif /* HAL_IRQLEVEL_H_ */
