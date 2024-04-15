/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.09.23
 */
#ifndef SRC_DRIVERS_INTERRUPT_PL190_H_
#define SRC_DRIVERS_INTERRUPT_PL190_H_

#include <assert.h>
#include <stdint.h>

#include "vic.h"
#include "vic_util.h"

#define VIC_IRQ_STATUS     0x00
#define VIC_FIQ_STATUS     0x04
#define VIC_RAW_STATUS     0x08
#define VIC_INT_SELECT     0x0c /* 1 = FIQ, 0 = IRQ */
#define VIC_INT_ENABLE     0x10 /* 1 = enable, 0 = no effect */
#define VIC_INT_DISABLE    0x14 /* 1 = disable, 0 = no effect */
#define VIC_SOFT_ENABLE    0x18 /* 1 = enable, 0 = no effect */
#define VIC_SOFT_DISABLE   0x1c /* 1 = disable, 0 = no effect */
#define VIC_PROTECT        0x20
#define VIC_ADDR           0x30 /* Currently active ISR */
#define VIC_DEF_ADDR       0x34 /* Default ISR */

#define VIC_VECT_ADDR(x)   (0x100 + (4 * (x))) /* (0..15) */
#define VIC_VECT_CTRL(x)   (0x200 + (4 * (x))) /* (0..15) */

#define VIC_VECT_CTRL_EN   (1U << 5)

#define VIC_VECT_IRQ_COUNT 16

static inline void __irqctrl_init(void) {
	int i, j;

	for (i = 0; i < VIC_DEVICE_COUNT; i++) {
		VIC_REG_STORE(i, VIC_INT_SELECT, 0);
		VIC_REG_STORE(i, VIC_INT_DISABLE, ~(uint32_t)0);
		VIC_REG_STORE(i, VIC_SOFT_DISABLE, ~(uint32_t)0);

		for (j = 0; j < VIC_VECT_IRQ_COUNT; j++) {
			VIC_REG_STORE(i, VIC_VECT_CTRL(j), 0);
		}
	}
}

static inline void __irqctrl_enable(unsigned int irq) {
	uint32_t vic_num;
	uint32_t irq_num;
	uint32_t regval;
	int i;

	vic_num = irq / VIC_IRQ_COUNT;
	irq_num = irq % VIC_IRQ_COUNT;

	regval = VIC_REG_LOAD(vic_num, VIC_INT_ENABLE);
	if (!(regval & (1U << irq_num))) {
		VIC_REG_STORE(vic_num, VIC_INT_ENABLE, 1U << irq_num);

		for (i = 0; i < VIC_VECT_IRQ_COUNT; i++) {
			regval = VIC_REG_LOAD(vic_num, VIC_VECT_CTRL(i));
			if (!(regval & VIC_VECT_CTRL_EN)) {
				VIC_REG_STORE(vic_num, VIC_VECT_CTRL(i),
				    VIC_VECT_CTRL_EN | irq_num);
				VIC_REG_STORE(vic_num, VIC_VECT_ADDR(i), irq);
			}
		}

		/* If false then all vectors are busy */
		assert(i != VIC_VECT_IRQ_COUNT);
	}
}

static inline void __irqctrl_disable(unsigned int irq) {
	uint32_t vic_num;
	uint32_t irq_num;
	uint32_t regval;
	int i;

	vic_num = irq / VIC_IRQ_COUNT;
	irq_num = irq % VIC_IRQ_COUNT;

	VIC_REG_STORE(vic_num, VIC_INT_DISABLE, 1U << irq_num);

	for (i = 0; i < VIC_VECT_IRQ_COUNT; i++) {
		regval = VIC_REG_LOAD(vic_num, VIC_VECT_CTRL(i));
		if ((regval & 0x3f) == (VIC_VECT_CTRL_EN | irq_num)) {
			VIC_REG_STORE(vic_num, VIC_VECT_CTRL(i), 0);
			VIC_REG_STORE(vic_num, VIC_VECT_ADDR(i), 0);
		}
	}
}

#endif /* SRC_DRIVERS_INTERRUPT_PL190_H_ */
