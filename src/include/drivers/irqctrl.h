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

/**
 * Enables the specified IRQ.
 *
 * @param irq the IRQ number to enable
 */
void irqctrl_enable(unsigned int irq);

/**
 * Disables the specified IRQ.
 *
 * @param irq the IRQ number to disable
 */
void irqctrl_disable(unsigned int irq);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param irq the IRQ number to clear
 */
void irqctrl_clear(unsigned int irq);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param irq the IRQ number to force
 */
void irqctrl_force(unsigned int irq);

#endif /* HAL_INTERRUPT_H_ */
