/**
 * @file irq_ctrl.h
 *
 * @brief Independent interface for interrupt controller.
 *
 * @details This file describes interface which every interrupt controller
 * must realize.
 *
 * @date 25.11.2009
 * @author Anton Bondarev
 */

#ifndef HAL_IRQ_CTRL_H_
#define HAL_IRQ_CTRL_H_

#include <asm/irq_ctrl.h>

/**
 * Initializes interrupt controller. Function is called by
 * @link hardware_init_hook() @endlink before another initialize function.
 *
 * @return result of initializing.
 * @retval 0 on success.
 * @retval -1 if error has been occured.
 */
int irqc_init(void);

/**
 * Sets pointed bit in irq_mask register.
 *
 * @param irq_num - irq number which you want to set.
 *
 * @return previous pointed bit state.
 * @retval -1 if error has been took place (wrong number for example).
 */
int irqc_enable_irq(irq_num_t irq_num);

/**
 * Clears pointed bit in irq_mask register.
 *
 * @param irq_num - irq number which you want to clear.
 *
 * @return previous pointed bit state.
 * @retval -1 if error has been took place (wrong number for example).
 */
int irqc_disable_irq(irq_num_t irq_num);

/**
 * Get status of the irq number specified.
 *
 * @param irq_num - irq number to check.
 *
 * @return irq_num previous pointed bit state.
 * @retval -1 if error has been took place (wrong number for example).
 */
int irqc_get_status(irq_num_t irq_num);

/**
 * Set irq_mask register in interrupt controller.
 *
 * @param irq mask - which you want to set.
 *
 * @return previous irq_mask state.
 */
irq_mask_t irqc_set_mask(irq_mask_t mask);

/**
 * Get irq_mask register in interrupt controller.
 *
 * @return irq_mask register state.
 */
irq_mask_t irqc_get_mask(void);

/**
 * Clear irq_mask register in interrupt controller. Clear pending register.
 *
 * @return irq_mask register state.
 */
int irqc_disable_all(void);

/**
 * Function set pointed bit in pending register.
 * if this interrupt is enabled interrupt was occured.
 *
 * @param irq_num - irq number which want to force.
 *
 * @return previous pointed bit state in pending register.
 * @retval -1 if error has been took place (wrong number for example).
 */
int irqc_force(irq_num_t irq_num);

/**
 * Function clear pointed bit in pending register.
 * If this interrupt is enabled interrupt was occured.
 *
 * @param irq_num -irq number which want to force.
 *
 * @return previous pointed bit state in pending register.
 * @retval -1 if error has been took place (wrong number for example).
 * */
int irqc_clear(irq_num_t irq_num);

#endif /* HAL_IRQ_CTRL_H_ */
