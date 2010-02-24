/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#ifndef SOFTIRQ_H_
#define SOFTIRQ_H_

/**
 * most significant soft irq
 */
#define SOFTIRQ_HI_LEVEL 0

/**
 * Total amount of possible soft IRQs.
 */
#define SOFTIRQ_NRS_TOTAL 32

/**
 * Checks if the specified softirq_nr represents valid soft IRQ number.
 */
#define softirq_nr_valid(softirq_nr) ((1 << softirq_nr))

/**
 * Type representing soft IRQ number.
 */
typedef unsigned char softirq_nr_t;

/**
 * Deferred Interrupt Service Routine type.
 *
 * @param softirq_nr the interrupt request number being handled
 * @param dev_id the device tag specified at #softirq_install() time
 */
typedef void (*softirq_handler_t)(softirq_nr_t softirq_nr, void *dev_id);

extern void softirq_init(void);

extern int softirq_install(softirq_nr_t nr, softirq_handler_t handler,
		void *dev_id);

extern int softirq_raise(softirq_nr_t nr);

#ifdef __KERNEL__
/**
 * Called by IRQ-related kernel code when leaving the interrupt context.
 */
extern void softirq_dispatch(void);
#endif /* __KERNEL__ */

#endif /* SOFTIRQ_H_ */
