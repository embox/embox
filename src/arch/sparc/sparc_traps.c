/**
 * @arch/sparc/sparc_traps.c
 * @brief function for working with traps in SPARC architecture
 * @date 24.11.2009
 * @author Anton Bondarev
 */
#include "types.h"

#include "asm/traps.h"
#include "kernel/irq.h"
#include "stdio.h"
#include "common.h"

// user trap handlers table
static IRQ_HANDLER user_trap_handlers[MAX_IRQ_NUMBER];


/**
 * Runs user defined handler (if one has been enabled).
 * @param tt - trap type (number of current trap) it's took from "tbr" register
 * @param sp - stack pointer (TODO why is stack pointer place here)
 */
void dispatch_trap(uint8_t tt, uint32_t *sp) {
	if (CHECK_IRQ_TRAP(tt)){
		irq_dispatch(IRQ_GET_NUMBER(tt));
		return ;
	}
	if (user_trap_handlers[tt] != NULL) {
		// fire user handler!
		user_trap_handlers[tt]();
	}
}



void dispatch_bad_trap(TRAP_CONTEXT * r, WORD tt) {
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

	while (1)
		;
}
