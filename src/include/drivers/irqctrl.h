/**
 * @file
 * @brief Interrupt controller interface.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef DRIVER_IRQCTRL_H_
#define DRIVER_IRQCTRL_H_

#include <stdint.h>

#include <module/embox/driver/interrupt/irqctrl_api.h>

/**
 * Total amount of interrupt lines that the controller can handle.
 *
 * @note Implementation should provide @c __IRQCTRL_IRQS_TOTAL definition
 * indicating positive constant.
 */
#define IRQCTRL_IRQS_TOTAL ((unsigned int)__IRQCTRL_IRQS_TOTAL)

/**
 * Enables the specified IRQ.
 *
 * @param irq the IRQ number to enable
 */
extern void irqctrl_enable(unsigned int irq);

/**
 * Enables the specified IRQ on a specific CPU (hart).
 *
 * @param hart_id the CPU (hart) ID
 * @param interrupt_nr the IRQ number to enable
 */
extern void irqctrl_enable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr);

/**
 * Disables the specified IRQ.
 *
 * @param irq the IRQ number to disable
 */
extern void irqctrl_disable(unsigned int irq);

/**
 * Disables the specified IRQ on a specific CPU (hart).
 *
 * @param hart_id the CPU (hart) ID
 * @param interrupt_nr the IRQ number to disable
 */
extern void irqctrl_disable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr);

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
 * Get pending status from irqctrl.
 *
 * @param irq the IRQ number status you want to know
 * @return non-zero if the IRQ is pending, 0 otherwise
 */
extern int irqctrl_pending(unsigned int irq);

/**
 * Signals end of interrupt for the specified IRQ.
 * Required by some interrupt controllers (e.g., x86).
 *
 * @param irq the IRQ number
 */
extern void irqctrl_eoi(unsigned int irq);

/**
 * Signals end of interrupt for the specified IRQ on a specific CPU (hart).
 * Required by some interrupt controllers (e.g., x86).
 *
 * @param hart_id the CPU (hart) ID
 * @param irq the IRQ number
 */
extern void irqctrl_eoi_in_cpu(uint32_t hart_id, unsigned int irq);

/**
 * Sets up interrupt priority.
 * A lower priority value indicates a higher interrupt priority.
 *
 * @param interrupt_nr the IRQ number
 * @param prio the priority value
 */
extern void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio);

/**
 * Gets the priority of the specified interrupt.
 * A lower priority value indicates a higher interrupt priority.
 *
 * @param interrupt_nr the IRQ number
 * @return the priority value
 */
extern unsigned int irqctrl_get_prio(unsigned int interrupt_nr);

/**
 * Gets the ID of the currently active interrupt.
 *
 * @retval -1 If there are no currently active interrupts
 * @return the interrupt ID, or -1 if none are active
 */
extern unsigned int irqctrl_get_intid(void);

/**
 * Gets the ID of the currently active interrupt on a specific CPU (hart).
 *
 * @param hart_id the CPU (hart) ID
 * @retval -1 If there are no currently active interrupts
 * @return the interrupt ID, or -1 if none are active
 */
extern unsigned int irqctrl_get_intid_from_cpu(uint32_t hart_id);

struct irqctrl {
	const char *name;
	int (*init)(void);
};

#define IRQCTRL_NAME __global_irqctrl

#define IRQCTRL_DEF(_name, _init)   \
	struct irqctrl IRQCTRL_NAME = { \
	    .name = #_name,             \
	    .init = _init,              \
	};

static inline int irqctrl_init(void) {
	extern struct irqctrl IRQCTRL_NAME;
	if (IRQCTRL_NAME.init) {
		return IRQCTRL_NAME.init();
	}
	return 0;
}

static inline struct irqctrl *irqctrl_get(void) {
	extern struct irqctrl IRQCTRL_NAME;
	return &IRQCTRL_NAME;
}

#endif /* DRIVER_IRQCTRL_H_ */
