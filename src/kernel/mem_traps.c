/**
 * \file mem_traps.c
 */

#include "kernel/irq.h"
#include "common.h"

void mem_not_aligned()
{
//	mem_access_error = true;
	LOG_ERROR("\n address isn't alligned\n");
}

void mem_access_exception()
{
//	mem_access_error = true;
	LOG_ERROR("\n memory access\n");
}

void mem_store_error()
{
//	mem_access_error = true;
	LOG_ERROR("\n writing error\n");
}

void mem_hang()
{
//	mem_access_error = true;
	LOG_ERROR("\n hand-up\n");
}

BOOL con_init() {
	irq_set_trap_handler(0x07, mem_not_aligned);
	irq_set_trap_handler(0x09, mem_access_exception);
	irq_set_trap_handler(0x2B, mem_store_error);
//	irq_set_handler(IRQ_Hang, mem_hang);
//	irq_set_handler(IRQ_Halt, halt_trap);
}
