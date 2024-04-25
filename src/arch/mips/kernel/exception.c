/**
 * @file
 * @brief
 *
 * @date 24.07.2012
 * @author Anton Bondarev
 */

#include <assert.h>
#include <string.h>

#include <asm/addrspace.h>
#include <asm/entry.h>
#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <embox/unit.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(mips_exception_init);

void mips_c_exception_handler(pt_regs_t *regs) {
	int i;

	printk("mips_c_exception_handler ptregs:\n");
	printk("gp (0x%lx); sp (0x%lx); fp(0x%lx)\n", regs->gp, regs->sp, regs->fp);
	printk("ra (0x%lx); lo (0x%lx); hi(0x%lx)\n", regs->ra, regs->lo, regs->hi);
	printk("cp0_status (0x%lx); pc (0x%lx);\n", regs->cp0_status, regs->pc);

	for (i = 0; i < 25; i++) {
		printk("reg[%d] = (0x%lx);\n", i, regs->reg[i]);
	}
	while (1) {}
	regs->pc += 4;
}

second_exception_handler_t exception_handlers[MIPS_EXCEPTIONS_QUANTITY] = {
    mips_c_exception_handler};

/*
* Copy the generic exception handlers to their final destination.
* This will be overridden later as suitable for a particular
* configuration.
*/
static void mips_setup_exc_table(void) {
	int i;

	/* set all exception handler */
	for (i = 0; i < MIPS_EXCEPTIONS_QUANTITY; i++) {
		exception_handlers[i] = mips_c_exception_handler;
	}
}

extern void flush_cache(unsigned long start_addr, size_t size);

/* setup correct exception table and enable exceptions */
static int mips_exception_init(void) {
	unsigned int tmp;

	/* clear BEV bit */
	tmp = mips_read_c0_status();
	tmp &= ~(ST0_BEV);
	mips_write_c0_status(tmp);

	/* clear CauseIV bit */
	tmp = mips_read_c0_cause();
	tmp &= ~(CAUSE_IV);
	mips_write_c0_cause(tmp);

	/* copy first exception handler to correct place */
	memcpy((void *)(KSEG0 + 0x180), &mips_first_exception_handler, 0x80);

	mips_setup_exc_table();

	flush_cache((unsigned long)(KSEG0 + 0x180), 0x80);

	execution_hazard_barrier();

	/* read status registers for cleaning interrupts mask */
	tmp = mips_read_c0_status();
	tmp &= ~(ST0_IM);          /* clear all interrupts mask */
	tmp &= ~ST0_IE;            /* global enable interrupt */
	mips_write_c0_status(tmp); /* write back status register */

	/* clear EXL bit */
	tmp = mips_read_c0_status();
	tmp &= ~(ST0_ERL);
	mips_write_c0_status(tmp);

	return 0;
}

void mips_exception_setup(mips_exception_type_t type,
    second_exception_handler_t handler) {
	exception_handlers[type] = handler;

	flush_cache((unsigned long)exception_handlers, sizeof(exception_handlers));
	execution_hazard_barrier();
}
