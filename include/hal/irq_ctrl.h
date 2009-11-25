/**
 * @file hal/irq_ctrl.h
 * @brief independent interface for interrupt controller
 * @details This file describes interface which every interrupt controller must realize
 * @date 25.11.2009
 * @author Anton Bondarev
 */

#ifndef HAL_IRQ_CTRL_H_
#define HAL_IRQ_CTRL_H_

#include "asm/irq_ctrl.h"

/**
 * Initialize interrupt controller. Function is called by hardware_init_hook before another initialize function.
 * @return 0 if success else -1
 */
int irqc_init();

/**
 * Set pointed bit in irq_mask register
 * @param irq number which you want to set
 * @return -1 if error took place  (wrong number for example) else previous pointed bit state
 */
int irqc_enable_irq(irq_num_t irq_num);

/**
 * Clear pointed bit in irq_mask register
 * @param irq number which you want to set
 * @return -1 if error took place (wrong number for example) else previous pointed bit state
 */
int irqc_disable_irq(irq_num_t irq_num);

/**
 * Get status of the irq number specified
 *
 * @param irq_num IRQ number to check
 * @return -1 if error took place (wrong number for example) else pointed bit state
 */
int irqc_get_status(irq_num_t irq_num);

/**
 * Set irq_mask register in interrupt controller
 * @param irq mask which you want to set
 * @return previous irq_mask state
 */
irq_mask_t irqc_set_mask(irq_mask_t mask);

/**
 * Get irq_mask register in interrupt controller
 * @return irq_mask register state
 */
irq_mask_t irqc_get_mask();

/**
 * Clear irq_mask register in interrupt controller. Clear pending register
 * @return irq_mask register state
 */
int irqc_disable_all();

/**
 * Function set pointed bit in pending register. if this interrupt is enabled interrupt was occured.
 * @param irq number which want to force
 * @return -1 if error took place (wrong number for example) else previous pointed bit state in pending register
 */
int irqc_force(irq_num_t irq_num);

/**
 * Function clear pointed bit in pending register. if this interrupt is enabled interrupt was occured.
 * @param irq number which want to force
 * @return -1 if error took place (wrong number for example) else previous pointed bit state in pending register
 */
int irqc_clear(irq_num_t irq_num);

#endif /* HAL_IRQ_CTRL_H_ */
