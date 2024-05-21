/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.05.24
 */

#include <assert.h>
#include <stdint.h>

#include <asm/mipsregs.h>
#include <drivers/irqctrl.h>
#include <hal/reg.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <util/log.h>

#ifdef MIPS_GIC_INTERRUPT_PIN
static void mips_gic_interrupt(void) {
	uint32_t pend;
	uint32_t mask;
	int i;

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i += 32) {
		pend = REG32_LOAD(MIPS_GIC_BASE + GIC_SH_PEND(i));
		mask = REG32_LOAD(MIPS_GIC_BASE + GIC_SH_MASK(i));

		log_debug("pend %x pask %x", pend, mask);
		pend &= mask;

		if (pend) {
			int j;
			for (j = 0; j < 32; j++) {
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

void mips_interrupt_handler(void) {
	unsigned int pending;
	unsigned int mask;
	unsigned int irq;

	//	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		pending = mips_read_c0_cause();
		mask = mips_read_c0_status();

		log_debug("interrupt occured c0_cause(%x) : c0_status(%x)",
		    mips_read_c0_cause(), mips_read_c0_status());

		pending &= (mask & ST0_IM);

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
