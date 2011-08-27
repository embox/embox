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

#ifndef KERNEL_IRQ_H_
#define KERNEL_IRQ_H_

#include <kernel/irq_lock.h>
#include <hal/interrupt.h>

/**
 * Total amount of possible IRQs in the system.
 * @note Equals to HAL #INTERRUPT_NRS_TOTAL value.
 */
#define IRQ_NRS_TOTAL \
	INTERRUPT_NRS_TOTAL

/**
 * Checks if the specified irq_nr represents valid IRQ number.
 * @note The same as HAL @link interrupt_nr_valid() @endlink macro.
 */
#define irq_nr_valid(irq_nr) \
	interrupt_nr_valid(irq_nr)

/**
 * Type representing interrupt request number.
 * @note The same as HAL #interrupt_nr_t type.
 */
typedef interrupt_nr_t irq_nr_t;

#define IRQ_NONE    0
#define IRQ_HANDLED 1

/**
 * IRQ handler return type. Must be either #IRQ_HANDLED or #IRQ_NONE.
 */
typedef int irq_return_t;

/**
 * Interrupt Service Routine type.
 *
 * @param irq_nr
 *   The number of interrupt request being handled.
 * @param data
 *   The device data specified at #irq_attach() time (if any).
 *
 * @return
 *   Interrupt handling result.
 * @retval IRQ_NONE
 *   If the ISR didn't actually handled this interrupt.
 * @retval IRQ_HANDLED
 *   If the interrupt has been successfully handled.
 */
typedef irq_return_t (*irq_handler_t)(irq_nr_t irq_nr, void *data);

/**
 * Initializes IRQ subsystem.
 */
extern void irq_init(void);

/**
 * Attaches an @link #irq_handler_t interrupt service routine @endlink to the
 * specified @link #irq_nr_t IRQ number @endlink.
 *
 * @param irq_nr
 *   The IRQ number to attach the @a handler to.
 * @param flags TODO not yet implemented -- Eldar
 * @param handler
 *   The ISR itself.
 * @param data
 *   The optional device data which will be passed to the ISR as the sole
 *   argument.
 * @param dev_name TODO the optional device name
 *
 * @return
 *   Result of attaching the specified handler.
 * @retval 0
 *   If everything is OK.
 * @retval -EINVAL
 *   If @a irq_nr is not @link #irq_nr_valid() valid @endlink, or
 *   if the @a handler is @c NULL.
 * @retval -EBUSY
 *   If another ISR has already been attached to the requested IRQ number.
 * @retval -ENOMEM
 *   If there is no memory to allocate internal structures for the new handler.
 * @retval -ENOSYS
 *   If kernel is compiled without IRQ support.
 */
extern int irq_attach(irq_nr_t irq_nr, irq_handler_t handler,
		unsigned int flags, void *data, const char *dev_name);

/**
 * Detaches ISR from the specified @link #irq_nr_t IRQ number @endlink.
 *
 * @param irq_nr
 *   The IRQ number to detach the ISR from.
 * @param data
 *   Device data specified at #irq_attach() time.
 *
 * @return
 *   Result of detaching.
 * @retval 0
 *   If all is OK.
 * @retval -EINVAL
 *   If @a irq_nr is not @link #irq_nr_valid() valid @endlink.
 * @retval -ENOENT
 *   If such handler have not been attached.
 * @retval -ENOSYS
 *   If kernel is compiled without IRQ support.
 */
extern int irq_detach(irq_nr_t irq_nr, void *data);

/**
 * Called by interrupt handler code.
 * @param interrupt_nr the number of interrupt to dispatch
 */
extern void irq_dispatch(interrupt_nr_t interrupt_nr);

#endif /* KERNEL_IRQ_H_ */
