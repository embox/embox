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
#include <drivers/irqctrl.h>
#include <module/embox/kernel/irq_api.h>

/** Total amount of IRQs supported by the system. */
#define IRQ_NRS_TOTAL \
	IRQCTRL_IRQS_TOTAL

/**
 * Checks if the specified @c irq_nr represents a valid IRQ number.
 */
#define irq_nr_valid(irq_nr) \
	((unsigned int) irq_nr < IRQ_NRS_TOTAL)

/**
 * IRQ flags
 */
/* Sharing supported flag */
#define IF_SHARESUP	(0x1 << 0)

/**
 * IRQ handler return type.
 */
typedef enum {
	IRQ_NONE    = 0, /**< Interrupt has not been handled. */
	IRQ_HANDLED = 1, /**< Interrupt has been processed by the handler*/
} irq_return_t;

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
 * @retval #IRQ_NONE
 *   If the ISR didn't actually handled this interrupt.
 * @retval #IRQ_HANDLED
 *   If the interrupt has been successfully handled.
 */
typedef irq_return_t (*irq_handler_t)(unsigned int irq_nr, void *data);

/**
 * Attaches an @link #irq_handler_t interrupt service routine @endlink to the
 * specified IRQ.
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
extern int irq_attach(unsigned int irq_nr, irq_handler_t handler,
		unsigned int flags, void *data, const char *dev_name);

/**
 * Detaches ISR from the specified IRQ.
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
extern int irq_detach(unsigned int irq_nr, void *data);

/**
 * Called by interrupt handler code.
 * @param interrupt_nr the number of interrupt to dispatch
 */
extern void irq_dispatch(unsigned int interrupt_nr);

/**
 * Enable interrupt of all attached IRQs. Mainly used in SMP for
 * APs initialization. This allows AP to handler interrupt independently
 */
extern void irq_enable_attached(void);

#ifndef STATIC_IRQ_EXTENTION
#define STATIC_IRQ_ATTACH(_irq_nr, _hnd, _data)
#endif

#endif /* KERNEL_IRQ_H_ */
