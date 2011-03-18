/**
 * @file
 * @brief Miscellaneous functions used in some tests.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef TEST_MISC_H_
#define TEST_MISC_H_

#include <kernel/irq.h>

/**
 * Recursively calls itself @a depth times.
 *
 * @param depth recursion depth
 * @return result recursive decrementing depth to zero
 * @retval zero always
 */
extern int test_misc_recursion(int depth);

/**
 * Forces the specified IRQ and checks whether the corresponding handler is
 * called.
 *
 * @note This function does exactly the same as #test_misc_irq_force_callback()
 *       called with @c NULL as its second argument.
 *
 * @param nr the number of the IRQ being forced
 * @return the force result
 * @retval 0 if the handler has been called
 * @retval -EIO if the IRQ handler has not been fired
 * @retval -EINVAL if @c irq_nr is not @link #irq_nr_valid() valid @endlink or
 *                 if the @c handler is @c NULL
 * @retval -EBUSY if another ISR has already been attached to the specified IRQ
 *                number
 * @retval -ENOSYS if kernel is compiled without IRQ support
 *
 * @see test_misc_irq_force_callback()
 */
extern int test_misc_irq_force(irq_nr_t nr);

/**
 * Forces the interrupt, checks whether the corresponding handler is called and
 * runs the specified callback inside the handler.
 *
 * @note This function does exactly the same as #test_misc_irq_force_callback()
 *       called with @c NULL as its second argument.
 *
 * @param nr the number of the IRQ being forced
 * @param cb the callback to invoke inside the ISR
 * @return the callback return value (if specified)
 * @retval 0 if the handler has been called but the passed @a cb is @c NULL
 * @retval -EIO if the IRQ handler has not been fired
 * @retval -EINVAL if @c irq_nr is not @link #irq_nr_valid() valid @endlink or
 *                 if the @c handler is @c NULL
 * @retval -EBUSY if another ISR has already been attached to the specified IRQ
 *                number
 * @retval -ENOSYS if kernel is compiled without IRQ support
 *
 * @see irq_attach()
 */
extern int test_misc_irq_force_callback(irq_nr_t nr, int (*cb)(void));

#endif /* TEST_MISC_H_ */
