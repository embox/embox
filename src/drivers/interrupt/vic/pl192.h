/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.09.23
 */
#ifndef SRC_DRIVERS_INTERRUPT_PL192_H_
#define SRC_DRIVERS_INTERRUPT_PL192_H_

#include <stdint.h>

#include "vic.h"
#include "vic_util.h"

#define VIC_IRQ_STATUS       0x00
#define VIC_FIQ_STATUS       0x04
#define VIC_RAW_STATUS       0x08
#define VIC_INT_SELECT       0x0c /* 1 = FIQ, 0 = IRQ */
#define VIC_INT_ENABLE       0x10 /* 1 = enable, 0 = no effect */
#define VIC_INT_DISABLE      0x14 /* 1 = disable, 0 = no effect */
#define VIC_SOFT_ENABLE      0x18 /* 1 = enable, 0 = no effect */
#define VIC_SOFT_DISABLE     0x1c /* 1 = disable, 0 = no effect */
#define VIC_PROTECT          0x20
#define VIC_VECT_PRIOR_DAISY 0x28
#define VIC_ADDR             0xf00 /* Currently active ISR */

#define VIC_VECT_ADDR(x)     (0x100 + (4 * (x))) /* (0..31) */
#define VIC_VECT_PRIOR(x)    (0x200 + (4 * (x))) /* (0..31) */

#define VIC_VECT_IRQ_COUNT   32

static inline void __irqctrl_init(void) {
	int i, j;

	for (i = 0; i < VIC_DEVICE_COUNT; i++) {
		VIC_REG_STORE(i, VIC_INT_SELECT, 0);
		VIC_REG_STORE(i, VIC_INT_DISABLE, ~(uint32_t)0);
		VIC_REG_STORE(i, VIC_SOFT_DISABLE, ~(uint32_t)0);
		VIC_REG_STORE(i, VIC_VECT_PRIOR_DAISY, 0xf);

		for (j = 0; j < VIC_VECT_IRQ_COUNT; j++) {
			VIC_REG_STORE(i, VIC_VECT_ADDR(j), (i * VIC_IRQ_COUNT) + j);
			VIC_REG_STORE(i, VIC_VECT_PRIOR(j), 0xf);
		}
	}
}

static inline void __irqctrl_enable(unsigned int irq) {
	VIC_REG_STORE(irq / VIC_IRQ_COUNT, VIC_INT_ENABLE,
	    1U << (irq % VIC_IRQ_COUNT));
}

static inline void __irqctrl_disable(unsigned int irq) {
	VIC_REG_STORE(irq / VIC_IRQ_COUNT, VIC_INT_DISABLE,
	    1U << (irq % VIC_IRQ_COUNT));
}

#endif /* SRC_DRIVERS_INTERRUPT_PL192_H_ */
