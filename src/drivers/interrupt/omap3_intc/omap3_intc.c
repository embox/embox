/**
 * @file
 * @brief OMAP35x interrupt controller driver
 *
 * @date 15.10.12
 * @author Ilia Vaprol
 */

#include <assert.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <hal/mmu.h>
#include <drivers/common/memory.h>
#include <drivers/irqctrl.h>
#include <mem/vmem.h>
#include <util/binalign.h>

#include <kernel/irq.h>
#include <embox/unit.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(omap3_intc_init);

/**
 * Specefic constants (n = 0 to 2, m = 0 to 95)
 */
#define OMAP35X_INTC_BASE           0x48200000
#define OMAP35X_INTC_REVISION       (OMAP35X_INTC_BASE + 0x00000000) /* REV 7:0b */
#define OMAP35X_INTC_SYSCONFIG      (OMAP35X_INTC_BASE + 0x00000010) /* SOFTRESET 1b, AUTOIDLE 0b */
#define OMAP35X_INTC_SYSSTATUS      (OMAP35X_INTC_BASE + 0x00000014) /* RESETDONE 0b */
#define OMAP35X_INTC_SIR_IRQ        (OMAP35X_INTC_BASE + 0x00000040) /* SPURIOUSIRQFLAG 31:7b, ACTIVEIRQ 6:0b */
#define OMAP35X_INTC_SIR_FIQ        (OMAP35X_INTC_BASE + 0x00000044) /* SPURIOUSFIQFLAG 31:7b, ACTIVEFIQ 6:0b */
#define OMAP35X_INTC_CONTROL        (OMAP35X_INTC_BASE + 0x00000048) /* NEWFIQAGR 1b, NEWIRQAGR 0b */
#define OMAP35X_INTC_PROTECTION     (OMAP35X_INTC_BASE + 0x0000004C) /* PROTECTION 0b */
#define OMAP35X_INTC_IDLE           (OMAP35X_INTC_BASE + 0x00000050) /* TURBO 1b, FUNCIDLE 0b */
#define OMAP35X_INTC_IRQ_PRIORITY   (OMAP35X_INTC_BASE + 0x00000060) /* SPURIOUSIRQFLAG 31:6b, IRQPRIORITY 5:0b */
#define OMAP35X_INTC_FIQ_PRIORITY   (OMAP35X_INTC_BASE + 0x00000064) /* SPURIOUSFIQFLAG 31:6b, FIQPRIORITY 5:0b */
#define OMAP35X_INTC_THRESHOLD      (OMAP35X_INTC_BASE + 0x00000068) /* PRIORITYTHRESHOLD 7:0b */
#define OMAP35X_INTC_ITR(n)         (OMAP35X_INTC_BASE + 0x00000080 + (n) * 0x20) /* ITR 31:0b */
#define OMAP35X_INTC_MIR(n)         (OMAP35X_INTC_BASE + 0x00000084 + (n) * 0x20) /* MIR 31:0b */
#define OMAP35X_INTC_MIR_CLEAR(n)   (OMAP35X_INTC_BASE + 0x00000088 + (n) * 0x20) /* MIRCLEAR 31:0b */
#define OMAP35X_INTC_MIR_SET(n)     (OMAP35X_INTC_BASE + 0x0000008C + (n) * 0x20) /* MIRSET 31:0b */
#define OMAP35X_INTC_ISR_SET(n)     (OMAP35X_INTC_BASE + 0x00000090 + (n) * 0x20) /* ISRSET 31:0b */
#define OMAP35X_INTC_ISR_CLEAR(n)   (OMAP35X_INTC_BASE + 0x00000094 + (n) * 0x20) /* ISRCLEAR 31:0b */
#define OMAP35X_INTC_PENDING_IRQ(n) (OMAP35X_INTC_BASE + 0x00000098 + (n) * 0x20) /* PENDINGIRQ 31:0b */
#define OMAP35X_INTC_PENDING_FIQ(n) (OMAP35X_INTC_BASE + 0x0000009C + (n) * 0x20) /* PENDINGFIQ 31:0b */
#define OMAP35X_INTC_ILR(m)         (OMAP35X_INTC_BASE + 0x00000100 + (m) * 0x4) /* PRIORITY 7:2b, FIQNIRQ 0b */

#define INTC_SYSCONFIG_RESET (1 << 1)
#define INTC_CONTROL_NEWIRQARG (1 << 0)
#define INTC_CONTROL_NEWFIQARG (1 << 1)
#define INTC_SIR_IRQ_ACTIVE_MASK 0x7f

void software_init_hook(void) {
	REG_STORE(OMAP35X_INTC_SYSCONFIG, INTC_SYSCONFIG_RESET);

	REG_STORE(OMAP35X_INTC_IDLE, 0x0);

	for (int m = 0; m < __IRQCTRL_IRQS_TOTAL; ++m) {
		REG_STORE(OMAP35X_INTC_ILR(m), 0x0);
	}

	REG_STORE(OMAP35X_INTC_MIR_SET(0), ~0);
	REG_STORE(OMAP35X_INTC_MIR_SET(1), ~0);
	REG_STORE(OMAP35X_INTC_MIR_SET(2), ~0);

	REG_STORE(OMAP35X_INTC_ISR_SET(0), 0);
	REG_STORE(OMAP35X_INTC_ISR_SET(1), 0);
	REG_STORE(OMAP35X_INTC_ISR_SET(2), 0);
}


static int omap3_intc_init(void) {
	return 0;
}

/* 32 bits per word;
 * interrupt_nr: lower 5 bits -- offset in word,
 * else -- register n
 */
void irqctrl_enable(unsigned int interrupt_nr) {
	REG_STORE(OMAP35X_INTC_MIR_CLEAR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_disable(unsigned int interrupt_nr) {
	REG_STORE(OMAP35X_INTC_MIR_SET(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_clear(unsigned int interrupt_nr) {
	REG_STORE(OMAP35X_INTC_ISR_CLEAR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_force(unsigned int interrupt_nr) {
	REG_STORE(OMAP35X_INTC_ISR_SET(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

int irqctrl_pending(unsigned int interrupt_nr) {
	return REG_LOAD(OMAP35X_INTC_PENDING_IRQ(interrupt_nr >> 5)) & 1 << (interrupt_nr & 0x1f);
}

void interrupt_handle(void) {
	unsigned int irq = REG_LOAD(OMAP35X_INTC_SIR_IRQ) & INTC_SIR_IRQ_ACTIVE_MASK;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);

	REG_STORE(OMAP35X_INTC_CONTROL, INTC_CONTROL_NEWIRQARG); /* reset IRQ output and enable new IRQ generation */

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();

	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

void swi_handle(void) {
	printk("swi!\n");
}

PERIPH_MEMORY_DEFINE(omap3_intc, OMAP35X_INTC_BASE, 0x1000);
