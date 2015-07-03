/**
 * @file
 * @brief Interrupt controller interface.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef DRIVER_IRQCTRL_H_
#define DRIVER_IRQCTRL_H_

#include <module/embox/driver/interrupt/irqctrl_api.h>

/**
 * Total amount of interrupt lines that the controller can handle.
 *
 * @note Implementation should provide @c __IRQCTRL_IRQS_TOTAL definition
 * indicating positive constant.
 */
#define IRQCTRL_IRQS_TOTAL \
	((unsigned int) __IRQCTRL_IRQS_TOTAL)

/**
 * Enables the specified IRQ.
 *
 * @param irq the IRQ number to enable
 */
extern void irqctrl_enable(unsigned int irq);

/**
 * Disables the specified IRQ.
 *
 * @param irq the IRQ number to disable
 */
extern void irqctrl_disable(unsigned int irq);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param irq the IRQ number to clear
 */
extern void irqctrl_clear(unsigned int irq);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param irq the IRQ number to force
 */
extern void irqctrl_force(unsigned int irq);

/**
 * Get pending status from irqctrl
 *
 * @param irq the IRQ number status you want to know
 */
extern int irqctrl_pending(unsigned int irq);

/**
 * Some interrupt controllers required end of interrupt, x86 for example
 */
extern void irqctrl_eoi(unsigned int irq);

#endif /* DRIVER_IRQCTRL_H_ */
