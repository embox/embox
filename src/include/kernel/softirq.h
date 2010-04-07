/**
 * @file
 * @brief Kernel software interrupts (a.k.a. deferred procedure call) handling.
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 *         - Initial API design
 * @author Dasha Teplyh
 *         - Documenting some parts of API
 */

#ifndef SOFTIRQ_H_
#define SOFTIRQ_H_

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
 * @param data the device tag specified at #softirq_install() time
 */
typedef void (*softirq_handler_t)(softirq_nr_t softirq_nr, void *data);

/**
 * Initializes soft IRQ subsystem.
 */
extern void softirq_init(void);

/**
 * Installs the handler on the specified soft IRQ number replacing an old one
 * (if any).
 *
 * @param nr the soft IRQ number to install handler on
 * @param handler the deferred ISR itself (can be @c NULL)
 * @param data the optional device tag which will be passed to the handler.
 *
 * @return installation result
 * @retval 0 if all is OK
 * @retval -EINVAL if the @c nr doesn't represent valid soft IRQ number
 *
 * @see #softirq_nr_valid()
 */
extern int softirq_install(softirq_nr_t nr, softirq_handler_t handler,
		void *data);

/**
 * Defers a call to the soft IRQ handler for the specified number (if any has
 * been installed earlier).
 *
 * @param nr the soft IRQ number
 *
 * @return the activation result
 * @retval 0 if the activation is done
 * @retval -EINVAL if the @c nr doesn't represent valid soft IRQ number
 */
extern int softirq_raise(softirq_nr_t nr);

#ifdef __KERNEL__
/**
 * Called by IRQ-related kernel code when leaving the interrupt context with
 * max IPL (all IRQ disabled).
 */
extern void softirq_dispatch(void);
#endif /* __KERNEL__ */

#endif /* SOFTIRQ_H_ */
