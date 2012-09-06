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
 * Type representing interrupt mask(pending) register.
 *
 * XXX outdated comment below... -- Eldar
 * @note Implementation should provide @c __unsigned int type indicating
 * unsigned (it is essential!) integer suitable to hold up to
 * (1 << #INTERRUPT_NRS_TOTAL)  values.
 */
typedef unsigned int interrupt_mask_t;

/**
 * Enables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to enable
 */
void interrupt_enable(unsigned int interrupt_nr);

/**
 * Disables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to disable
 */
void interrupt_disable(unsigned int interrupt_nr);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to clear
 */
void interrupt_clear(unsigned int interrupt_nr);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to force
 */
void interrupt_force(unsigned int interrupt_nr);

/**
 * Receives status of interrupt controller.
 *
 * @return pending register from interrupt controller
 */
interrupt_mask_t interrupt_get_status(void);

extern int i8259_irq_pending(unsigned int irq);

#endif /* HAL_INTERRUPT_H_ */
