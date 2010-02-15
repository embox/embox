/**
 * @file
 * @brief Kernel interrupt requests handling.
 *
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Alexandr Batyukov, Alexey Fomin
 *         - Reviewing and rewriting some parts
 * @author Eldar Abusalimov
 *         - Rewriting from scratch, interface change
 */

#ifndef IRQ_H_
#define IRQ_H_
#include <autoconf.h>
#include <types.h>

#include <hal/interrupt.h>

/**
 * Total amount of possible IRQs in the system.
 * @note Equals to HAL @link INTERRUPT_NRS_TOTAL @endlink value.
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
 * @note The same as HAL @link interrupt_nr_t @endlink type.
 */
typedef interrupt_nr_t irq_nr_t;

/**
 * IRQ handler return type.
 */
typedef bool irq_return_t;

/**
 * Interrupt Service Routine type.
 *
 * @param irq_nr the interrupt request number being handled
 * @param dev_id the device tag specified at @link irq_attach() @endlink time
 *
 * @return IRQ handling result
 * @retval IRQ_NONE if ISR didn't handled the interrupt
 * @retval IRQ_HANDLED if interrupt has been handled by this ISR
 */
typedef irq_return_t (*irq_handler_t)(irq_nr_t irq_nr, void *dev_id);

typedef unsigned long irq_flags_t;

#if 0
struct irq_info {
	irq_nr_t irq_nr;
	irq_handler_t handler;
	irq_flags_t flags;
	void *dev_id;
	const char* dev_name;
	unsigned int count;
};
#endif

void irq_init(void);
int irq_attach(irq_nr_t irq_nr, irq_handler_t handler, irq_flags_t flags,
		void *dev_id, const char *dev_name);
int irq_detach(irq_nr_t irq_nr, void *dev_id);

#if 0
int irq_info(irq_nr_t irq_nr, struct irq_info *info);
#endif

#ifdef __HAL__
/**
 * Called by HAL code.
 * @param interrupt_nr the number of interrupt to dispatch
 */
void irq_dispatch(interrupt_nr_t interrupt_nr);
#endif /* __HAL__ */

#endif /* IRQ_H_ */
