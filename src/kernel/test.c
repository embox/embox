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
    IRQ_HANDLER user_trap_handlers[ALLOWED_TRAPS_AMOUNT];
    //SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];
}context;

static int timers_context;
void save_context()
{
	context.irqmask = irq_regs->mask;
    context.cache_ctrl = l_regs->cachectrl;

	memcpy (context.user_trap_handlers, user_trap_handlers, sizeof (user_trap_handlers));
	//memcpy (context.sys_timers, sys_timers, sizeof (sys_timers));
	timers_context = timers_context_save ();
}

void restore_context ()
{
	int i;
	irq_regs->mask           = 0x00000000;
	irq_regs->clear          = 0xFFFFFFFF;

	memcpy (user_trap_handlers, context.user_trap_handlers, sizeof (user_trap_handlers));
	//memcpy (sys_timers, context.sys_timers, sizeof (sys_timers));
	timers_context_restore (timers_context);
	irq_regs->mask = context.irqmask;
    l_regs->cachectrl = context.cache_ctrl;
}


void test_abort_handler() {
	char ch;
//	if (!chproc_started)
//		return;
//	printf("\nHELLO             TEST_ABORT_HANDLER            \n");

	ch = uart_getc();
	if ((ch == 'Q') || ch == 'q')
	{
		//printf("\nHELLO             TEST_ABORT_HANDLER            \n");
		chproc_abort_accept = TRUE;

	}
}


void test_allow_aborting()
{
	uart_set_irq_handler(test_abort_handler);
}
void test_disable_aborting ()
{
	uart_remove_irq_handler(test_abort_handler);
}

void test_run(WORD pfunc)
{
	chproc_abort_accept = FALSE;
	save_context ();
	start_child_process(pfunc);
	restore_context ();
}
