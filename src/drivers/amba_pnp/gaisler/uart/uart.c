/**
 * \file uart.c
 */

#include "irq.h"
#include "asm/leon_config.h"
//#include "asm/memory_map.h"
#include "amba_pnp.h"
#include "uart.h"
#include "common.h"

#define UART_RX_READY                   0x00000001
#define UART_TX_READY                   0x00000004      /*< hold register empty */
// control register bit masks
#define UART_DISABLE_ALL                0x00000000      /*< disable all */
#define UART_RX_ENABLE                  0x00000001      /*< reciever enable */
#define UART_TX_ENABLE                  0x00000002      /*< transmitter enable */
#define UART_INT_RX_ENABLED             0x00000004
#define UART_INT_TX_ENABLED             0x00000004

static volatile UART_STRUCT * dev_regs = NULL;

#include "amba_drivers_helper.h"
#undef module_init
#define module_init() uart_init()

static int irq;

static void irq_func_uart() {
	char ch = uart_getc();
}

int uart_init() {
	if (NULL != dev_regs)
		return -1;

	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_UART);

	if (UART_BASE != amba_dev.bar[0].start) {
        TRACE("uart base is %x instead of correct value %x\n",
                amba_dev.bar[0].start, UART_BASE);
        return -1;
    }
    if (UART_IRQ != amba_dev.dev_info.irq) {
        TRACE("uart irq is %d instead of correct value %d",
                amba_dev.dev_info.irq, UART_IRQ);
        return -1;
    }

	dev_regs = (UART_STRUCT * )amba_dev.bar[0].start;
	irq = amba_dev.dev_info.irq;
	/**< disabled uart */
	dev_regs->ctrl = 0x0;
	dev_regs->scaler = UART_SCALER_VAL;
	/**< enable uart */
	dev_regs->ctrl = /*UART_INT_RX_ENABLED |*/ UART_TX_ENABLE | UART_RX_ENABLE;
#ifndef SIMULATION_TRG
	//while (!(UART_TX_READY & dev_regs->status));
	//clear uart
	//while (UART_RX_READY & dev_regs->status);
	//uart->data;
	//irq_set_handler(IRQ_UART1, irq_func_uart);
#endif
}
/*
BOOL uart_is_empty() {
	if (UART_RX_READY & uart->status)
		return FALSE;
	else
		return TRUE;
}
*/

void uart_putc(char ch) {
	int i;
	if (NULL == dev_regs)
		uart_init();
/*
#ifndef SIMULATION_TRG
	while (!(UART_TX_READY & dev_regs->status))
		;
#endif
*/
	for (i=0;i < 0x1000; i ++);

	dev_regs->data = (UINT32)ch;
}

char uart_getc() {
	CHECK_INIT_MODULE();

	while (!(UART_RX_READY & dev_regs->status))
		;
	return ((char) dev_regs->data);
}

int uart_set_irq_handler(IRQ_HANDLER pfunc) {
	CHECK_INIT_MODULE();

	dev_regs->ctrl |= UART_INT_RX_ENABLED ;
	irq_set_handler(irq, pfunc);
	return 0;
}

int uart_remove_irq_handler(IRQ_HANDLER pfunc) {
	CHECK_INIT_MODULE();

	dev_regs->ctrl &= ~UART_INT_RX_ENABLED ;
	irq_set_handler(irq, NULL);
	return 0;
}

