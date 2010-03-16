/**
 * @file
 *
 * @date 15.03.2010
 * @author Nikolay Korotky
 */
#include <common.h>
#include <types.h>
#include <asm/winmacro.h>
#include <asm/ptrace.h>

void bad_trap_handler(uint8_t tt, struct pt_regs *r) {
	struct reg_window *rw;

	TRACE("BAD TRAP (0x%02x)\n", tt);

	TRACE("PSR: %08x PC: %08x NPC: %08x Y: %08x\n",
			r->psr, r->pc, r->npc, r->y);
	TRACE("%%G: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			r->globals[0], r->globals[1], r->globals[2], r->globals[3],
			r->globals[4], r->globals[5], r->globals[6], r->globals[7]);
	TRACE("%%O: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			r->ins[0], r->ins[1], r->ins[2], r->ins[3],
			r->ins[4], r->ins[5], r->ins[6], r->ins[7]);

	rw = (struct reg_window *) r->ins[6];

	TRACE("%%L: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			rw->locals[0], rw->locals[1], rw->locals[2], rw->locals[3],
			rw->locals[4], rw->locals[5], rw->locals[6], rw->locals[7]);
	TRACE("%%I: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
			rw->ins[0], rw->ins[1], rw->ins[2], rw->ins[3],
			rw->ins[4], rw->ins[5], rw->ins[6], rw->ins[7]);

	while (1)
		;
}
