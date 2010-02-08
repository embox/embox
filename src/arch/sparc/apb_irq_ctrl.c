/**
 * @file irq_ctrl.c
 *
 * @brief Realize hal layer interrupt ctrl for leon 3 processor
 * @date 14.04.09
 * @author Anton Bondarev
 */

#include "hal/irq_ctrl.h"
#include "assert.h"
#include "types.h"
#include "common.h"
#include "drivers/amba_pnp.h"

typedef struct _IRQ_REGS {
	volatile unsigned int level;    /* 0x00 */
	volatile unsigned int pend;     /* 0x04 */
	volatile unsigned int force;    /* 0x08 */
	volatile unsigned int clear;    /* 0x0C */
	volatile unsigned int mpstatus; /* 0x10 */
	volatile unsigned int dummy[11];/* 0x14 - 0x3C */
	volatile unsigned int mask;     /* 0x40 */
} IRQ_REGS;

static IRQ_REGS * dev_regs = NULL;

#define ASSERT_INIT_DONE() assert_not_null(dev_regs)


int irqc_init(void) {
#ifndef SIMULATION_TRG
	AMBA_DEV amba_dev;
	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_INTERRUPT_UNIT);
	dev_regs = (IRQ_REGS *) amba_dev.bar[0].start;
#else
	dev_regs = (IRQ_REGS *) IRQ_CTRL_BASE;
#endif

	REG_STORE(dev_regs->level, 0);
	REG_STORE(dev_regs->mask, 0);
	REG_STORE(dev_regs->pend, 0);
	REG_STORE(dev_regs->force, 0);
	REG_STORE(dev_regs->clear, 0xFFFFFFFF);
	return 0;
}

int irqc_enable_irq(irq_num_t irq_num) {
	ASSERT_INIT_DONE();
	set_bit(dev_regs->mask, irq_num);
	return 0;
}

int irqc_disable_irq(irq_num_t irq_num) {
	ASSERT_INIT_DONE();
	clear_bit(dev_regs->mask, irq_num);
	return 0;
}

int irqc_disable_all(void) {
	ASSERT_INIT_DONE();
	REG_STORE(dev_regs->mask, 0);
	return 0;
}

int irqc_force(irq_num_t irq_num) {
//	That's force!
//	if (!get_bit(dev_regs->mask, irq_num)) {
//		return;
//	}
	set_bit(dev_regs->force, irq_num);
	return 0;
}

int irqc_clear(irq_num_t irq_num) {
	ASSERT_INIT_DONE();
	set_bit(dev_regs->clear, irq_num);
	return 0;
}

int irqc_get_status(irq_num_t irq_num) {
	return get_bit(dev_regs->mask, irq_num);
}

irq_mask_t irqc_set_mask(irq_mask_t mask){
	irq_mask_t old_mask = REG_LOAD(dev_regs->mask);
	REG_STORE(dev_regs->mask, mask);
	return old_mask;
}

irq_mask_t irqc_get_mask(void){
	return REG_LOAD(dev_regs->mask);
}
