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

#include "common.h"
#include "kernel/irq_ctrl.h"
#include "asm/traps.h"
#include "kernel/irq.h"
#include "asm/sys.h"

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

void dispatch_bad_trap(TRAP_CONTEXT * r, WORD tt) {
#ifdef MONITOR_ARCH_SPARC
	REG_WINDOW *rw;

	TRACE("BAD TRAP (0x%02x)\n", tt);

	TRACE("PSR: %08x PC: %08x NPC: %08x Y: %08x\n",
			r->psr, r->pc, r->npc, r->y);
	TRACE("%%G: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			r->globals[0], r->globals[1], r->globals[2], r->globals[3],
			r->globals[4], r->globals[5], r->globals[6], r->globals[7]);
	TRACE("%%O: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			r->ins[0], r->ins[1], r->ins[2], r->ins[3],
			r->ins[4], r->ins[5], r->ins[6], r->ins[7]);

	rw = (REG_WINDOW *) r->ins[6];

	TRACE("%%L: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			rw->locals[0], rw->locals[1], rw->locals[2], rw->locals[3],
			rw->locals[4], rw->locals[5], rw->locals[6], rw->locals[7]);
	TRACE("%%I: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			rw->ins[0], rw->ins[1], rw->ins[2], rw->ins[3],
			rw->ins[4], rw->ins[5], rw->ins[6], rw->ins[7]);
#endif /* MONITOR_ARCH_SPARC */
	while (1)
		;
}

void irq_init_handlers() {
	int i;

	irq_ctrl_init();
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

BOOL irq_set_info(IRQ_INFO *irq_info) {
	IRQ_INFO old_irq_info;
	unsigned long psr;
	// check IRQ number
	if (irq_info->irq_num != irq_info->irq_num & 0xF) {
		return FALSE;
	}
	if (irq_info->enabled && irq_info->handler == NULL) {
		return FALSE;
	}

	psr = local_irq_save();
	  old_irq_info.irq_num = irq_info->irq_num;
	  old_irq_info.handler = user_trap_handlers[IRQ_TRAP_TYPE(irq_info->irq_num)];
	  old_irq_info.enabled = irq_ctrl_get_status(irq_info->irq_num);

	  user_trap_handlers[IRQ_TRAP_TYPE(irq_info->irq_num)] = irq_info->handler;
	  if (irq_info->enabled) {
		  irq_ctrl_enable_irq(irq_info->irq_num);
	  } else {
		  irq_ctrl_disable_irq(irq_info->irq_num);
	  }
	  memcpy(irq_info, &old_irq_info, sizeof(IRQ_INFO));
	  irq_ctrl_clear(irq_info->irq_num);
	local_irq_restore(psr);

	return TRUE;
}

int request_irq(BYTE irq_number, IRQ_HANDLER handler) {
	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return;
	}

	if ((NULL == handler) || (NULL != user_trap_handlers[IRQ_TRAP_TYPE(irq_number)])){
		return -1;
	}
	//TODO may be clear pending bit?
	user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
	irq_ctrl_enable_irq(irq_number);
	return 0;
}

void irq_set_handler(BYTE irq_number, IRQ_HANDLER pfunc) {
	// check IRQ number
	if (irq_number != irq_number & 0xF) {
		return;
	}

	//	IRQ_HANDLER old = user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
	user_trap_handlers[IRQ_TRAP_TYPE(irq_number)] = pfunc;
	if (pfunc != NULL) {
		LOG_DEBUG("set irq=%d\n", irq_number);
		irq_ctrl_enable_irq(irq_number);
	} else {
		irq_ctrl_disable_irq(irq_number);
	}
}

IRQ_HANDLER irq_get_handler(BYTE irq_number) {
	return user_trap_handlers[IRQ_TRAP_TYPE(irq_number)];
}
