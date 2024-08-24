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
 * NOTICE
 * In order to better support SMP, the propagation of external interrupts
 * is explained below in more detail.
 * There are external interrupt sources, one or more, level or edge sensitive
 * If they want to interrupt the execution of the CPU, these steps are required:
 *   1. First, the interrupt controller must be set to recognize an external **signal**
 *   2. Then the interrupt controller must forward **request** to the CPUs
 *
 * [signal] here refers to the information passed externally to the interrupt controller
 * [request] here refers to the information passed by the interrupt controller to the target(CPU)
 * one signal can be forwarded as many requests to many targets(CPUs)
 *
 */

/**
 * Enables the specified IRQ source in interrupt controller
 * Let the controller start detecting external interrupt signals
 *
 * By default, it also enables interrupt request to forward
 * to a target which calls this function
 *
 * @param irq the IRQ number to enable
 */
extern void irqctrl_enable(unsigned int irq);

/**
 * This function configures where the interrupt signal will be passed
 * as interrupt request to. It does NOT change whether the interrupt controller
 * is able to detect the signal
 *
 * @param hart_id interrupt request should (also) forward to this hartid CPU
 * @param interrupt_nr the IRQ number to enable
 */
extern void irqctrl_enable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr);

/**
 * Disables the specified IRQ source in interrupt controller
 * so interrupt controller can not forward interrupt signal to
 * any target
 *
 * @param irq the IRQ number to disable
 */
extern void irqctrl_disable(unsigned int irq);

/**
 * This function configures where the interrupt signal will NOT be passed
 * as interrupt request to. It does NOT change whether the interrupt controller
 * is able to detect the signal
 *
 * @param hart_id interrupt request should NOT forward to this hartid CPU
 * @param interrupt_nr the IRQ number to disable
 */
extern void irqctrl_disable_in_cpu(uint32_t hart_id, unsigned int interrupt_nr);

/**
 * Clears pending status for the specified IRQ signal
 *
 * @param irq the IRQ number to clear
 */
extern void irqctrl_clear(unsigned int irq);

/**
 * Forces interrupt controller to generate the specified IRQ signal
 * Which CPU it is propagated to depends on the interrupt controller settings
 *
 * @param irq the IRQ number to force
 */
extern void irqctrl_force(unsigned int irq);

/**
 * Get pending status of this interrupr signal from irqctrl.
 *
 * @param irq the IRQ number status you want to know
 * @return non-zero if the IRQ is pending, 0 otherwise
 */
extern int irqctrl_pending(unsigned int irq);

/**
 * Indicates end of interrupt for the specified IRQ.
 * Required by some interrupt controllers (e.g., x86).
 *
 * @param irq the IRQ number
 */
extern void irqctrl_eoi(unsigned int irq);

/**
 * Indicates end of interrupt for the specified IRQ on a specific CPU (hart).
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
