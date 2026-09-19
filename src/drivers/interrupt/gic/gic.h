/**
 * @file
 *
 * @date 26 march 2016
 * @author: Anton Bondarev
 */
#ifndef DRIVERS_INTERRUPT_GIC_GIC_H_
#define DRIVERS_INTERRUPT_GIC_GIC_H_

/* A board with interrupts above 255 raises it from its build.conf:
 * CFLAGS += -D__IRQCTRL_IRQS_TOTAL=512 */
#ifndef __IRQCTRL_IRQS_TOTAL
#define __IRQCTRL_IRQS_TOTAL 256
#endif

#endif /* DRIVERS_INTERRUPT_GIC_GIC_H_ */
