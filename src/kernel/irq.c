/**
 * \file irq.c
 *
 * \brief Common interrupting-related handling routines
 *
 * \author Eldar Abusalimov
 * \author Anton Bondarev
 * \author Alexey Fomin
 * \author Alexandr Batyukov
 */

#include "types.h"
#include "common.h"
#include "irq_ctrl.h"
#include "traps.h"
#include "irq.h"

// user trap handlers table
static IRQ_HANDLER user_trap_handlers[IRQ_TABLE_SIZE];

/*
 * Runs user defined handler (if one has been enabled).
 */
void dispatch_trap(BYTE tt, UINT32 *sp) {
	if (user_trap_handlers[tt] != NULL) {
		// fire user handler!
		user_trap_handlers[tt]();
	}
}

void dispatch_bad_trap(int tt, int pc, int npc, int psr) {
	TRACE("! tt: %x, pc: %x, npc: %x, psr: %x\n", tt, pc, npc, psr);
	while (1)
		;
}

void irq_init_handlers() {
	int i;

	irq_ctrl_init();

	for (i = 0; i < IRQ_TABLE_SIZE; i++) {
		user_trap_handlers[i] = NULL;
	}

}

BOOL irq_set_trap_handler(BYTE tt, IRQ_HANDLER pfunc) {

	if (NULL == user_trap_handlers[tt]) {
		// set handler
		user_trap_handlers[tt] = pfunc;
		return TRUE;
	}

	return FALSE;
}

BOOL irq_remove_trap_handler(BYTE tt) {

	if (NULL != user_trap_handlers[tt]) {
		// remove handler
		user_trap_handlers[tt] = NULL;
		return TRUE;
	}

	return FALSE;
}

void irq_set_handler(BYTE irq_number, IRQ_HANDLER pfunc) {
	LOG_DEBUG("set irq=%d\n", irq_number);
	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return;
	}

	IRQ_HANDLER old = user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
	user_trap_handlers[IRQ_TRAP_TYPE(irq_number)] = pfunc;

	if (pfunc != NULL && old == NULL) {
		irq_ctrl_enable_irq(irq_number);
	} else if (pfunc == NULL && old != NULL) {
		irq_ctrl_disable_irq(irq_number);
	}

}

IRQ_HANDLER irq_get_handler(BYTE irq_number) {
	return user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
}
