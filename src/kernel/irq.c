#include "types.h"
#include "leon.h"
#include "irq_ctrl.h"
#include "irq.h"
#include "test.h"
#include "common.h"
#include "memory_map.h"

//IRQ_REGS * const irq_regs = (IRQ_REGS * const ) IRQ_REGS_BASE;

// user trap handlers table
IRQ_HANDLER user_trap_handlers[ALLOWED_TRAPS_AMOUNT];

/*
 * Runs user defined handler (if one has been enabled).
 */
void dispatch_trap(BYTE tt) {
	if (user_trap_handlers[tt] != NULL) {
		// fire user handler!
		user_trap_handlers[tt]();
	}
	if (chproc_abort_accept) {
		chproc_abort_accept = FALSE;
		__asm__("call shell_handler_continue\n\t");
		__asm__("nop");
	}
}

void dispatch_bad_trap(int tt, int pc, int npc, int psr) {
	printf("! tt: %x, pc: %x, npc: %x, psr: %x\n", tt, pc, npc, psr);
	while(1);
}

void irq_init_handlers() {
	int i;
//	irq_regs->level = 0;
//	irq_regs->mask = 0;
//	irq_regs->pend = 0;
//	irq_regs->force = 0;
//	irq_regs->clear = 0xFFFFFFFF;

	irq_ctrl_init();

	for (i = 0; i < ALLOWED_TRAPS_AMOUNT; i++) {
		user_trap_handlers[i] = NULL;
	}

}

/*
 * set trap handler
 */
BOOL irq_set_trap_handler(BYTE tt, IRQ_HANDLER pfunc) {

	if (NULL == user_trap_handlers[tt]) {
		// set handler
		user_trap_handlers[tt] = pfunc;
		return TRUE;
	}

	return FALSE;
}

/*
 * remove trap handler
 */
BOOL irq_remove_trap_handler(BYTE tt) {

	if (NULL != user_trap_handlers[tt]) {
		// remove handler
		user_trap_handlers[tt] = NULL;
		return TRUE;
	}

	return FALSE;
}

/*
 * set interrupt handler
 */
BOOL irq_set_handler(BYTE irq_number, IRQ_HANDLER pfunc) {

	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return FALSE;
	}

	if (irq_set_trap_handler(IRQ_TRAP_TYPE(irq_number),pfunc)) {
		// enable interrupt
		//SetBit(irq_regs->mask, irq_number);
		irq_ctrl_enable_irq(irq_number);
		return TRUE;
	}

	return FALSE;
}

/*
 * remove interrupt handler
 */
BOOL irq_remove_handler(BYTE irq_number) {

	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return FALSE;
	}

	if (irq_remove_trap_handler(IRQ_TRAP_TYPE(irq_number))) {
		// disable interrupt
		//ClearBit(irq_regs->mask, irq_number);
		irq_ctrl_disable_irq(irq_number);
		return TRUE;
	}

	return FALSE;
}
