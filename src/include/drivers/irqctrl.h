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
 * Enables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to enable
 */
void irqctrl_enable(unsigned int interrupt_nr);

/**
 * Disables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to disable
 */
void irqctrl_disable(unsigned int interrupt_nr);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to clear
 */
void irqctrl_clear(unsigned int interrupt_nr);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to force
 */
void irqctrl_force(unsigned int interrupt_nr);

#endif /* HAL_INTERRUPT_H_ */
