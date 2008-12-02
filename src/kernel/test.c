#include "types.h"
#include "irq.h"
#include "test.h"
#include "conio.h"
#include "leon.h"
#include "common.h"
#include "timers.h"
#include "uart.h"



// child process return state
volatile UINT32 chproc_fp;
// child process started
volatile BOOL chproc_started;
// child process return value
volatile BOOL chproc_ret_val;
// accept or not process aborting
volatile BOOL chproc_abort_accept;

struct programm_context
{
	UINT32 irqmask;
    UINT32 cache_ctrl;
	IRQ_HANDLER_DESCR irq_handlers_table[IRQ_HTBL_SIZE];
	SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];
}context;


void save_context()
{
	context.irqmask = irq_regs->mask;
    context.cache_ctrl = l_regs->cachectrl;

	memcpy (context.irq_handlers_table, irq_handlers_table, sizeof (irq_handlers_table));
	memcpy (context.sys_timers, sys_timers, sizeof (sys_timers));
}

void restore_context ()
{
	int i;
	irq_regs->mask           = 0x00000000;
	irq_regs->clear          = 0xFFFFFFFF;
	irq_current = 0;

	memcpy (irq_handlers_table, context.irq_handlers_table, sizeof (irq_handlers_table));
	memcpy (sys_timers, context.sys_timers, sizeof (sys_timers));
	irq_regs->mask = context.irqmask;
    l_regs->cachectrl = context.cache_ctrl;
}


/*
void test_abort_handler()
{
	char ch;

	if (!chproc_started)
		return;

	ch = uart_getc();
	if ((ch == 'Q') || ch == 'q')
	{
		chproc_abort_accept = TRUE;
	}
}


void test_allow_aborting()
{
	irq_set_handler(IRQ_UART1, test_abort_handler);
}
void test_disable_aborting ()
{
	irq_remove_handler (IRQ_UART1);
}
*/
void test_run(WORD pfunc)
{
	chproc_abort_accept = FALSE;
	save_context ();
	start_child_process(pfunc);
	restore_context ();
}
