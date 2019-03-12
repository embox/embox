/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */
#include <assert.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <asm/ptrace.h>
#include <util/log.h>

#include <drivers/interrupt/lapic/regs.h>

#define MAX_TIRS 8 /* Don't know maximum TIRS count, so define 8 */
#define GET_NR_TIRS(tir_hi)		((tir_hi >> 56) & 0xff)

#define E2K_EXP_INTERRUPT  (1UL << 32)

struct e2k_tir {
	uint64_t lo;
	uint64_t hi;
};

static void e2k_get_tirs(struct e2k_tir *tirs) {
	int nr_tirs = -1;

	log_debug("TIR regiters:\n");

	do {
		++nr_tirs;
		assert(nr_tirs < MAX_TIRS);

		tirs[nr_tirs].hi = E2K_GET_DSREG(tir.hi);
		tirs[nr_tirs].lo = E2K_GET_DSREG(tir.lo);
		log_debug("TIR%d: lo=0x%lx, hi=0x%lx\n",
			nr_tirs, tirs[nr_tirs].lo, tirs[nr_tirs].hi);
	} while(GET_NR_TIRS(tirs[nr_tirs].hi));

	E2K_SET_DSREG(tir.lo, tirs[nr_tirs].lo);
}

static void e2k_exception_handler(struct e2k_tir tir0) {
	printk("e2k_exception_handler:\n");
	printk("  TIRO: lo=0x%lx, hi=0x%lx\n", tir0.lo, tir0.hi);
}

static void e2k_interrupt_handler(void) {
	int irq;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irq = lapic_get_irq();

	log_debug(">> handle irq %d\n", irq);

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		irq_dispatch(irq);

		irqctrl_eoi(irq);
	}
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

void e2k_trap_handler(struct pt_regs *regs) {
	struct e2k_tir tirs[MAX_TIRS];

	e2k_get_tirs(tirs);

	if (tirs[0].hi & E2K_EXP_INTERRUPT) {
		e2k_interrupt_handler();
	} else {
		e2k_exception_handler(tirs[0]);
	}
}
