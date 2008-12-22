#include "types.h"
#include "leon.h"
#include "irq.h"
#include "test.h"
#include "common.h"
#include "memory_map.h"

IRQ_REGS * const irq_regs = (IRQ_REGS * const ) IRQ_REGS_BASE;

/*
 * some registers to restore after user handler has completed
 */
typedef struct {

	WORD psr;
	WORD pc;
	WORD npc;
	WORD tbr;

} IRQ_RESTORE_INFO;

//int irq_current_pointer;

// PSR register
WORD irq_psr;
// PC register
WORD irq_pc;
// nPC register
WORD irq_npc;
// trap base register
WORD irq_tbr;

// user trap handlers table
IRQ_HANDLER user_trap_handlers[ALLOWED_TRAPS_AMOUNT];

// if this flag is enabled then skip IRQ dispatching
// and do not enter dispatch_trap()
BOOL irq_ignore;

/*
 * Saves irq_psr, irq_pc, irq_npc and irq_tbr global variables to the locals.
 * Then runs user defined handler (if one has been enabled).
 * After all restores these global variables and returns.
 */
void dispatch_trap() {
	IRQ_RESTORE_INFO restore_info;
	BYTE tt;

	restore_info.npc = irq_npc;
	restore_info.pc = irq_pc;
	restore_info.psr = irq_psr;
	restore_info.tbr = irq_tbr;

	tt = (irq_tbr >> 4) & 0xFF;

	// interrupts are enabled while user handler is being performed
	irq_ignore = FALSE;

	if (user_trap_handlers[tt] != NULL) {
		// fire user handler!
		user_trap_handlers[tt]();
	}

	irq_ignore = TRUE;

	irq_npc = restore_info.npc;
	irq_pc = restore_info.pc;
	irq_psr = restore_info.psr;
	irq_tbr = restore_info.tbr;

}

void irq_init_handlers() {
	int i;
	irq_regs->level = 0;
	irq_regs->mask = 0;
	irq_regs->pend = 0;
	irq_regs->force = 0;
	irq_regs->clear = 0xFFFFFFFF;

	for (i = 0; i < ALLOWED_TRAPS_AMOUNT; i++) {
		user_trap_handlers[i] = NULL;
	}

	irq_ignore = FALSE;
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
		SetBit(irq_regs->mask, irq_number);
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
		ClearBit(irq_regs->mask, irq_number);
		return TRUE;
	}

	return FALSE;
}
