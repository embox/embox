#include "types.h"
#include "leon.h"
#include "irq.h"
#include "test.h"
#include "common.h"
#include "memory_map.h"

IRQ_REGS *const irq_regs = (IRQ_REGS *const) IRQ_REGS_BASE;
// Current free place in irq buffers
int irq_current;

// Interrupt buffers
WORD irq_tbl_pc[IRQ_BUFFER_SIZE];   // return PC
WORD irq_tbl_npc[IRQ_BUFFER_SIZE];  // return nPC
WORD irq_tbl_psr[IRQ_BUFFER_SIZE];  // return PSR
WORD irq_tbl_tt[IRQ_BUFFER_SIZE];   // trap type

// Irq handler table
IRQ_HANDLER_DESCR irq_handlers_table[IRQ_HTBL_SIZE];



void irq_manager()
{
	BYTE tt;
	int i;

	//assert(irq_current > 0);
	if (!(irq_current > 0))
		return;
	// get irq type
	tt = (BYTE)irq_tbl_tt[irq_current-1];
	// find irq handler
	for (i = 0; i < IRQ_HTBL_SIZE; i++)
	{
		if (irq_handlers_table[i].tt == tt)
		{
			// run irq handler
			if (irq_handlers_table[i].pfunc != NULL)
				irq_handlers_table[i].pfunc();

			// check if child process running
			if (!chproc_started)
				chproc_abort_accept = FALSE;

			return;
		}
	}
}

void irq_init_handlers()
{
	int i;
	irq_regs->level = 0;
	irq_regs->mask = 0;
	irq_regs->pend = 0;
	irq_regs->force = 0;
	irq_regs->clear = 0xFFFFFFFF;

	for (i = 0; i < IRQ_HTBL_SIZE; i++)
		irq_handlers_table[i].tt = IRQ_NO_TRAP;

	chproc_started = FALSE;
}

BOOL irq_set_trap_handler(BYTE tt, IRQ_HANDLER pfunc)
{
	int i;
	// find free place (or tt place) in table
	for (i = 0; i < IRQ_HTBL_SIZE
	              && irq_handlers_table[i].tt != tt
	              && irq_handlers_table[i].tt != IRQ_NO_TRAP;
	              i++)
		;
	if (i < IRQ_HTBL_SIZE)
	{
		// set handler
		irq_handlers_table[i].tt = tt;
		irq_handlers_table[i].pfunc = pfunc;
		return TRUE;
	}
	return FALSE;
}

// set interrupt handler
BOOL irq_set_handler(BYTE nirq, IRQ_HANDLER pfunc)
{
	int tt;

	// check irq number
	if (nirq > 15)
		return FALSE;
	//assert(nirq > 0 && nirq < 16);

	// calculate trap type
	tt = IRQ_TRAP_TYPE(nirq);
	if (irq_set_trap_handler(tt, pfunc))
	{
		// enable interrupt
		SetBit(irq_regs->mask, nirq);
		return TRUE;
	}
	return FALSE;
}
// remove interrupt handler
void irq_remove_handler(BYTE nirq)
{
	int i, tt;
	// check irq number
	if (nirq > 15)
		return;
//	assert(nirq > 0 && nirq < 16);
	// calculate trap type
	tt = IRQ_TRAP_TYPE(nirq);
	// find free place (or tt place) in table
	for (i = 0; i < IRQ_HTBL_SIZE
				&& irq_handlers_table[i].tt != tt;
				i++)
		;

	if (i < IRQ_HTBL_SIZE)
	{
		// remove handler
		irq_handlers_table[i].tt = IRQ_NO_TRAP;
		// disable interrupt
		ClearBit(irq_regs->mask, nirq);
	}
}
