/**
 * @file
 *
 * @date Mar 23, 2020
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stdint.h>

#include <asm/io.h>
#include <asm/mipsregs.h>
#include <drivers/mips/global_control_block.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <util/log.h>

#include "mips_gic_regs.h"

#define MIPS_GIC_BASE OPTION_GET(NUMBER, base_addr)

static int mips_gic_init(void) {
	uint32_t c0;
	int i;

#if (MIPS_GIC_BASE < (0xA0000000))
	mips32_gcb_set_register(GCR_GIC_BASE, MIPS_GIC_BASE | GIC_EN);
#else
	mips32_gcb_set_register(GCR_GIC_BASE,
	    (MIPS_GIC_BASE - (0xA0000000)) | GIC_EN);
#endif /* (MIPS_GIC_BASE < (0xA0000000)) */
	__sync();

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i++) {
		if (0 == (i & 0x1F)) {
			REG32_STORE((MIPS_GIC_BASE + GIC_SH_RMASK(i)), 0xFFFFFFFF);
			REG32_STORE(MIPS_GIC_BASE + GIC_SH_POL(i), 0xFFFFFFFF);
		}
		/* only 1 cpu core */
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_MAP_CORE31_0(i), 0x1);
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_MAP_PIN(i),
		    MIPS_GIC_INTERRUPT_PIN | 1u << 31);
	}

	c0 = mips_read_c0_status();
	c0 |= 1 << (MIPS_GIC_INTERRUPT_PIN + ST0_IRQ_MASK_OFFSET + ST0_SOFTIRQ_NUM);
	mips_write_c0_status(c0);

	log_info("mips_gic config %x\n", REG_LOAD(MIPS_GIC_BASE + GIC_SH_CONFIG));
	log_info("mips_gic revision %x\n", REG_LOAD(MIPS_GIC_BASE + GIC_SH_REVID));

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	uint32_t mask;
	uint32_t reg;
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < 8) {
		c0 = mips_read_c0_status();
		c0 |= 1 << (irq + ST0_IRQ_MASK_OFFSET);
		mips_write_c0_status(c0);
	}
	else {
		reg = irq >> 5;
		mask = (1 << (irq & 0x1F));
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_SMASK(reg << 5), mask);
	}
}

void irqctrl_disable(unsigned int irq) {
	uint32_t mask;
	uint32_t reg;
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < 8) {
		c0 = mips_read_c0_status();
		c0 &= ~(1 << (irq + ST0_IRQ_MASK_OFFSET));
		mips_write_c0_status(c0);
	}
	else {
		reg = irq >> 5;
		mask = (1 << (irq & 0x1F));
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_RMASK(reg << 5), mask);
	}
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
	uint32_t mask;
	uint32_t reg;

	assert(irq_nr_valid(irq));

	/* XXX */
	if (irq >= 8) {
		reg = irq >> 5;
		mask = (1 << (irq & 0x1F));
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_SMASK(reg << 5), mask);
	}
}

unsigned int irqctrl_get_intid(void) {
	uint32_t pending;
	int i, j;

	pending = (mips_read_c0_cause() & CAUSE_IM) >> ST0_IRQ_MASK_OFFSET;

	if (!(pending & (0x1 << (ST0_SOFTIRQ_NUM + MIPS_GIC_INTERRUPT_PIN)))) {
		for (i = 0; i < 8; i++) {
			if (pending & (1U << i)) {
				return i;
			}
		}
	}

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i += 32) {
		pending = REG32_LOAD(MIPS_GIC_BASE + GIC_SH_PEND(i));
		pending &= REG32_LOAD(MIPS_GIC_BASE + GIC_SH_MASK(i));

		if (!pending) {
			continue;
		}

		for (j = 0; j < 32; j++) {
			if (pending & (1U << j)) {
				return i + j;
			}
		}
	}

	return -1;
}

IRQCTRL_DEF(mips_gic, mips_gic_init);
