/**
 * @file
 * @brief OMAP35x interrupt controller driver
 *
 * @date 15.10.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <types.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <drivers/irqctrl.h>

#include <kernel/irq.h>
#include <embox/unit.h>
#include <prom/prom_printf.h>

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

void hardware_init_hook(void) {
	prom_printf("hw hook\n");
	REG_STORE(OMAP35X_INTC_SYSCONFIG, 0x0);
#if 0
	REG_STORE(OMAP35X_INTC_IDLE, 0x0);

	for (int m = 0; m <= 95; ++m) {
		REG_STORE(OMAP35X_INTC_ILR(m), 0x0);
	}

	REG_STORE(OMAP35X_INTC_MIR_SET(0), ~0);
	REG_STORE(OMAP35X_INTC_MIR_SET(1), ~0);
	REG_STORE(OMAP35X_INTC_MIR_SET(2), ~0);
#endif
#if 0 /* software intrerrupt */
	REG_STORE(OMAP35X_INTC_ISR_SET(0), ~0);
	REG_STORE(OMAP35X_INTC_ISR_SET(1), ~0);
	REG_STORE(OMAP35X_INTC_ISR_SET(2), ~0);
#endif
}


static int omap3_intc_init(void) {
#if 0
	int n, m;

	REG_STORE(OMAP35X_INTC_SYSCONFIG, 0x1);
	REG_STORE(OMAP35X_INTC_IDLE, 0x0);

	for (m = 0; m <= 95; ++m) {
		REG_STORE(OMAP35X_INTC_ILR(m), 0x0);
	}

	for (n = 0; n <= 2; ++n) {
		REG_STORE(OMAP35X_INTC_MIR(n), 0x0);
		REG_STORE(OMAP35X_INTC_MIR_CLEAR(n), 0x1);
	}

#endif
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
}

void irqctrl_disable(unsigned int interrupt_nr) {
}

void irqctrl_clear(unsigned int interrupt_nr) {
}

void irqctrl_force(unsigned int interrupt_nr) {
}

void interrupt_handle(void) {
	prom_printf("irq 0x%x!\n", (unsigned int) REG_LOAD(OMAP35X_INTC_SIR_IRQ));
	for (int i = 0; i <= 2; i ++) {
	    prom_printf("pending %d: 0x%x\n", i, (unsigned int) REG_LOAD(OMAP35X_INTC_PENDING_IRQ(i)));
	}
	REG_STORE(OMAP35X_INTC_CONTROL, 0x1); /* reset IRQ output and enable new IRQ generation */
}

void swi_handle(void) {
	prom_printf("swi!\n");
	for (int i = 0; i <= 2; i ++) {
	    prom_printf("   active %d: 0x%x\n", i, (unsigned int) REG_LOAD(OMAP35X_INTC_ISR_SET(i)));
	}
}

