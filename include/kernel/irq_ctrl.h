/**
 * \file irq_ctrl.h
 *
 * \date Apr 14, 2009
 * \author anton
 */

#ifndef IRQ_CTRL_H_
#define IRQ_CTRL_H_

/**
 * init hardware irq ctrl
 * Caprute APB dev
 * init pointer to struct regs
 * @return 0 if success
 * @return -1 another way
 */
int irq_ctrl_init();

/*
 * Enable IRQ by it number.
 */
int irq_ctrl_enable_irq(int irq_num);

/*
 * Disable IRQ by it number.
 */
int irq_ctrl_disable_irq(int irq_num);

/*
 * Disable all IRQs.
 */
int irq_ctrl_disable_all();

#endif /* IRQ_CTRL_H_ */

