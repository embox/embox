/**
 * \file irq_ctrl.h
 *
 * \date Apr 14, 2009
 * \author anton
 */

#ifndef IRQ_CTRL_H_
#define IRQ_CTRL_H_
#if 0
/*this code move to hal folder*/
/**
 * init hardware irq ctrl
 * Caprute APB dev
 * init pointer to struct regs
 * @return 0 if success
 * @return -1 another way
 */
int irqc_init();

/*
 * Enable IRQ by it number.
 */
int irqc_enable_irq(int irq_num);

/*
 * Disable IRQ by it number.
 */
int irqc_disable_irq(int irq_num);

/*
 * Disable all IRQs.
 */
int irqc_disable_all();

/**
 * Calls interrupt specified - right at the moment (forced)
 * ( + no matter whether it was enabled or not)
 *
 * @param irq_num IRQ number to force call
 */
int irqc_force(BYTE irq_num);

/**
 * Clears irq state (deletes unhandled irq)
 *
 * @param irq_num IRQ number to clear
 */
int irqc_clear(BYTE irq_num);

/**
 * Get status of the irq specified
 *
 * @param irq_num IRQ number to check
 * @return whether it is enabled or not
 */
int irqc_get_status(BYTE irq_num);
#endif

#endif /* IRQ_CTRL_H_ */

