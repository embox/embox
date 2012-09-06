/**
 * @file
 * @brief Interrupt controller interface.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_INTERRUPT_H_
#define HAL_INTERRUPT_H_

#include <module/embox/driver/interrupt/irqctrl_api.h>
#include <arch/interrupt.h>

/**
 * Type representing interrupt line number.
 *
 * XXX outdated comment below... -- Eldar
 * @note Implementation should provide @c __interrupt_nr_t type indicating
 * unsigned (it is essential!) integer suitable to hold up to
 * #INTERRUPT_NRS_TOTAL values.
 */
typedef unsigned int interrupt_nr_t;

/**
 * Type representing interrupt mask(pending) register.
 *
 * XXX outdated comment below... -- Eldar
 * @note Implementation should provide @c __interrupt_nr_t type indicating
 * unsigned (it is essential!) integer suitable to hold up to
 * (1 << #INTERRUPT_NRS_TOTAL)  values.
 */
typedef unsigned int interrupt_mask_t;

/**
 * Enables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to enable
 */
void interrupt_enable(interrupt_nr_t interrupt_nr);

/**
 * Disables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to disable
 */
void interrupt_disable(interrupt_nr_t interrupt_nr);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to clear
 */
void interrupt_clear(interrupt_nr_t interrupt_nr);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to force
 */
void interrupt_force(interrupt_nr_t interrupt_nr);

/**
 * Receives status of interrupt controller.
 *
 * @return pending register from interrupt controller
 */
interrupt_mask_t interrupt_get_status(void);

extern int i8259_irq_pending(interrupt_nr_t irq);

#endif /* HAL_INTERRUPT_H_ */
