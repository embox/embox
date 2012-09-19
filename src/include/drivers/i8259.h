/**
 * @file
 * @brief i8259 Programmable Interrupt Controller.
 *
 * @date 06.01.11
 * @author Anton Bondarev
 */

#ifndef I8259_H_
#define I8259_H_

extern int i8259_irq_pending(unsigned int irq);
extern void i8259_send_eoi(unsigned int irq);

#endif /* I8259_H_ */
