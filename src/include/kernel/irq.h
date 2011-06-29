/**
 * @file
 * @brief Kernel interrupt requests handling.
 *
 * @date 26.08.09
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Alexandr Batyukov, Alexey Fomin
 *         - Reviewing and rewriting some parts
 * @author Eldar Abusalimov
 *         - Rewriting from scratch, interface change
 */

#ifndef IRQ_H_
#define IRQ_H_

#include <stdbool.h>
#include <hal/interrupt.h>

/**
 * Total amount of possible IRQs in the system.
 * @note Equals to HAL #INTERRUPT_NRS_TOTAL value.
 */
#define IRQ_NRS_TOTAL INTERRUPT_NRS_TOTAL

/**
 * Checks if the specified irq_nr represents valid IRQ number.
 * @note The same as HAL @link interrupt_nr_valid() @endlink macro.
 */
#define irq_nr_valid(irq_nr) interrupt_nr_valid(irq_nr)

#define IRQ_NONE    (false)
#define IRQ_HANDLED (true)

/**
 * Type representing interrupt request number.
 * @note The same as HAL #interrupt_nr_t type.
 */
typedef interrupt_nr_t irq_nr_t;

/**
 * IRQ handler return type. Must be either #IRQ_HANDLED or #IRQ_NONE.
 */
typedef bool irq_return_t;

/**
 * Interrupt Service Routine type.
 *
 * @param irq_nr the number of interrupt request being handled
 * @param data the device tag specified at @link irq_attach() @endlink time
 *
 * @return interrupt handling result
 * @retval IRQ_NONE if ISR didn't handled the interrupt
 * @retval IRQ_HANDLED if interrupt has been handled by this ISR
 */
typedef irq_return_t (*irq_handler_t)(irq_nr_t irq_nr, void *data);

typedef unsigned long irq_flags_t;

/**
 * Initializes IRQ subsystem.
 */
extern void irq_init(void);

/**
 * Attaches @link #irq_handler_t interrupt service routine @endlink to the
 * specified @link #irq_nr_t IRQ number @endlink.
 *
 * @param irq_nr the IRQ number to attach the @c handler to
 * @param handler the ISR itself
 * @param flags TODO not yet implemented -- Eldar
 * @param data the optional device tag which will be passed to the ISR.
 * @param dev_name the optional device name
 *
 * @return attach result
 * @retval 0 if all is OK
 * @retval -EINVAL if @c irq_nr is not @link #irq_nr_valid() valid @endlink or
 *                 if the @c handler is @c NULL
 * @retval -EBUSY if another ISR has already been attached to the specified IRQ
 *                number
 * @retval -ENOSYS if kernel is compiled without IRQ support
 */
extern int irq_attach(irq_nr_t irq_nr, irq_handler_t handler,
		irq_flags_t flags, void *data, const char *dev_name);

/**
 * Detaches ISR from the specified @link #irq_nr_t IRQ number @endlink.
 *
 * @param irq_nr the IRQ number to detach ISR from
 * @param data device tag specified at #irq_attach() time
 *
 * @return detach result
 * @retval 0 if all is OK
 * @retval -EINVAL if @c irq_nr is not @link #irq_nr_valid() valid @endlink
 * @retval -ENOSYS if kernel is compiled without IRQ support
 */
extern int irq_detach(irq_nr_t irq_nr, void *data);

/**
 * Called by interrupt handler code.
 * @param interrupt_nr the number of interrupt to dispatch
 */
extern void irq_dispatch(interrupt_nr_t interrupt_nr);

#endif /* IRQ_H_ */
