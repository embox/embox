/**
 * @file
 * @brief
 *
 * @date 07.08.2012
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <assert.h>
#include <asm/mipsregs.h>
#include <kernel/irq.h>
#include <asm/ptrace.h>
#include <asm/entry.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(mips_interrupt_init);

#include <hal/reg.h>
#include <stdint.h>

#ifdef MIPS_GIC_INTERRUPT_PIN
void mips_gic_interrupt(void) {
	uint32_t pend;
	int i;

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i += 32) {
		pend = REG32_LOAD(MIPS_GIC_BASE + GIC_SH_PEND(i)) &
				REG32_LOAD(MIPS_GIC_BASE + GIC_SH_MASK(i));

		if (pend) {
			int j;
			for (j = 0; j < 32; j ++) {
				if (pend & (1 << j)) {
//					irqctrl_disable(i + j);
					ipl_enable();
					irq_dispatch(i + j);
					ipl_disable();
//					irqctrl_enable(i + j);
				}
			}
		}
	}
}
#endif

void mips_c_interrupt_handler(pt_regs_t *regs) {

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		unsigned int pending = mips_read_c0_cause() & mips_read_c0_status() & ST0_IM;

#ifdef MIPS_GIC_INTERRUPT_PIN

		pending >>= ST0_IRQ_MASK_OFFSET;
		if (pending & (0x1 << MIPS_IRQN_TIMER)) {
					/* timer */
//					irqctrl_disable(MIPS_IRQN_TIMER);
					ipl_enable();
					irq_dispatch(MIPS_IRQN_TIMER);
					ipl_disable();
//					irqctrl_enable(MIPS_IRQN_TIMER);
		}
		if (pending & (0x1 << (ST0_SOFTIRQ_NUM + MIPS_GIC_INTERRUPT_PIN))) {
			mips_gic_interrupt();
		}
#else
		unsigned int irq;

		for (irq = 15; irq > 7; irq--) {
			if (pending & (1 << irq)) {

				irqctrl_disable(irq - 8);
				ipl_enable();
				irq_dispatch(irq - 8);
				ipl_disable();
				irqctrl_enable(irq - 8);
			}
		}
#endif
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

/**
 * Initialize MIPS build-in interrupts controller
 */
static int mips_interrupt_init(void) {
	unsigned int c0_reg;

	/* set interrupt handler exception */
	mips_exception_setup(MIPS_EXCEPTION_TYPE_IRQ, mips_c_interrupt_handler);

	/* read status registers for cleaning interrupts mask */
	c0_reg = mips_read_c0_status();
	c0_reg &= ~(ST0_IM);           /* clear all interrupts mask */
	c0_reg |= ST0_IE;              /* global enable interrupt */
	mips_write_c0_status(c0_reg);  /* write back status register */

	/* read cause register for cleaning all pending bits */
	c0_reg = mips_read_c0_cause();
	c0_reg &= ~(ST0_IM);           /* clear all interrupts pending bits */
	mips_write_c0_cause(c0_reg);   /* write back cause register */

	return 0;
}
