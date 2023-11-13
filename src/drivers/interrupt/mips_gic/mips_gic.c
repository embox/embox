/**
 * @file
 *
 * @date Mar 23, 2020
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#include <drivers/mips/global_control_block.h>
#include <hal/reg.h>
#include <asm/mipsregs.h>
#include <kernel/irq.h>

#include <asm/io.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(mips_gic_init);

static int mips_gic_ctrl_init(void) {
	int i;

#if (MIPS_GIC_BASE < (0xA0000000))
	mips32_gcb_set_register(GCR_GIC_BASE, MIPS_GIC_BASE | GIC_EN);
#else
	mips32_gcb_set_register(GCR_GIC_BASE, (MIPS_GIC_BASE - (0xA0000000)) | GIC_EN);
#endif /* (MIPS_GIC_BASE < (0xA0000000)) */
	__sync();

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i++) {
		if (0 == (i & 0x1F)) {
			REG32_STORE((MIPS_GIC_BASE + GIC_SH_RMASK(i)), 0xFFFFFFFF);
			REG32_STORE(MIPS_GIC_BASE + GIC_SH_POL(i), 0xFFFFFFFF);
		}
		 /* only 1 cpu core */
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_MAP_CORE31_0(i), 0x1);
		REG32_STORE(MIPS_GIC_BASE + GIC_SH_MAP_PIN(i), MIPS_GIC_INTERRUPT_PIN | 1u << 31);
	}
	return 0;
}

static int mips_gic_init(void) {
	uint32_t c0;

	c0 = mips_read_c0_status();
	c0 |= (1 << (MIPS_GIC_INTERRUPT_PIN + ST0_IRQ_MASK_OFFSET + ST0_SOFTIRQ_NUM));
	mips_write_c0_status(c0);

	log_info("mips_gic config %x\n", REG_LOAD(MIPS_GIC_BASE + GIC_SH_CONFIG));
	log_info("mips_gic revision %x\n", REG_LOAD(MIPS_GIC_BASE + GIC_SH_REVID));
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	int reg;
	uint32_t mask;

	log_debug("enabling %d", interrupt_nr);
	if (interrupt_nr >= __IRQCTRL_IRQS_TOTAL) {
		return;
	}
	if (interrupt_nr < 16) {
		uint32_t c0;

		c0 = mips_read_c0_status();
		c0 |= 1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET);
		mips_write_c0_status(c0);
		return;
	}

	reg = interrupt_nr >> 5;
	mask = (1 << (interrupt_nr & 0x1F));

	REG32_STORE(MIPS_GIC_BASE + GIC_SH_SMASK(reg << 5), mask);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	int reg;
	uint32_t mask;

	if (interrupt_nr >= __IRQCTRL_IRQS_TOTAL) {
		return;
	}
	if (interrupt_nr < 16) {
		uint32_t c0;

		c0 = mips_read_c0_status();
		c0 &= ~(1 << (interrupt_nr + ST0_IRQ_MASK_OFFSET));
		mips_write_c0_status(c0);
		return;
	}

	reg = interrupt_nr >> 5;
	mask = (1 << (interrupt_nr & 0x1F));

	REG32_STORE(MIPS_GIC_BASE + GIC_SH_RMASK(reg << 5), mask);
}

IRQCTRL_DEF(mips_gic, mips_gic_ctrl_init);
