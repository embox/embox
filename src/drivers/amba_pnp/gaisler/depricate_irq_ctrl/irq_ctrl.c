/**
 * \file irq_ctrl.c
 *
 * \date Apr 14, 2009
 * \author Anton Bondarev
 * \brief Realize hal layer interupt ctrl for leon 3 processor
 */

#include "kernel/irq_ctrl.h"

#include "types.h"
#include "common.h"
#include "drivers/amba_pnp.h"

typedef struct _IRQ_REGS {
	volatile unsigned int level; /* 0x00 */
	volatile unsigned int pend; /* 0x04 */
	volatile unsigned int force; /* 0x08 */
	volatile unsigned int clear; /* 0x0C */
	volatile unsigned int mpstatus; /* 0x10 */
	volatile unsigned int dummy[11]; /* 0x14 - 0x3C */
	volatile unsigned int mask; /* 0x40 */
} IRQ_REGS;

static IRQ_REGS * dev_regs = NULL;

#define ASSERT_INIT_DONE() assert_not_null(dev_regs)

static AMBA_DEV amba_dev;

int irqc_init() {
#ifndef SIMULATION_TRG
	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_INTERRUPT_UNIT);
#else
	amba_dev.bar[0].start = IRQ_CTRL_BASE;
#endif

	dev_regs = (IRQ_REGS *) amba_dev.bar[0].start;

	REG_STORE(dev_regs->level, 0);
	REG_STORE(dev_regs->mask, 0);
	REG_STORE(dev_regs->pend, 0);
	REG_STORE(dev_regs->force, 0);
	REG_STORE(dev_regs->clear, 0xFFFFFFFF);
	return 0;
}

int irqc_enable_irq(int irq_num) {
	ASSERT_INIT_DONE();
	SetBit(dev_regs->mask, irq_num);
	return 0;
}

int irqc_disable_irq(int irq_num) {
	ASSERT_INIT_DONE();
	ClearBit(dev_regs->mask, irq_num);
	return 0;
}

int irqc_disable_all() {
	ASSERT_INIT_DONE();
	REG_STORE(dev_regs->mask, 0);
	return 0;
}

int irqc_force(BYTE irq_num) {
//	That's force!
//	if (!GetBit(dev_regs->mask, irq_num)) {
//		return;
//	}
	SetBit(dev_regs->force, irq_num);
	return 0;
}

int irqc_clear(BYTE irq_num) {
	ASSERT_INIT_DONE();
	SetBit(dev_regs->clear, irq_num);
	return 0;
}

int irqc_get_status(BYTE irq_num) {
	return GetBit(dev_regs->mask, irq_num);
}


