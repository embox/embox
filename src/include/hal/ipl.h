/**
 * @file
 * @brief Interrupt Priority Level
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_IPL_H_
#define HAL_IPL_H_

#include <asm/hal/ipl.h>

/**
 * The implementation-dependent type suitable to hold IPL flags.
 */
typedef __ipl_t ipl_t;

/**
 * Called by kernel code after all necessary preparations are done and we ready
 * to handle interrupts.
 * @note Implementation should set the lowest interrupt priority level allowing
 * all IRQs.
 */
void ipl_init(void);

/**
 * Set the lowest interrupt priority level allowing all IRQs.
 *
 * @sa ipl_disable()
 */
#define ipl_enable()  ipl_init()

/**
 * Sets the maximal interrupt priority level cutting off all IRQs on the CPU.
 *
 * @sa ipl_enable()
 * @sa ipl_save()
 */
#define ipl_disable() ((void) ipl_save())


/**
 * Sets the maximal interrupt priority level cutting off all IRQs on the CPU.
 *
 * @return the magic value used to restore previous state with @link
 * ipl_restore() @endlink .
 *
 * @sa ipl_restore()
 */
ipl_t ipl_save(void);

/**
 * Restores the previous interrupt level specified by the @c ipl argument.
 *
 * @param ipl the value returned by @link ipl_save() @endlink method
 *              invocation
 *
 * @sa ipl_save()
 */
void ipl_restore(ipl_t ipl);

#endif /* HAL_IPL_H_ */
