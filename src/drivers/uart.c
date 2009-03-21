#include "types.h"
#include "irq.h"
#include "leon_config.h"
//#include "memory_map.h"
#include "plug_and_play.h"
#include "pnp_id.h"
#include "uart.h"

typedef struct _UART_STRUCT {
	volatile UINT32 data; //0x70
	volatile UINT32 status;
	volatile UINT32 ctrl;
	volatile UINT32 scaler;
} UART_STRUCT;

#define UART_RX_READY           0x00000001      //
#define UART_TX_READY           0x00000004      // hold register empty
// control register bit masks
#define UART_DISABLE_ALL			0x00000000      // disable all
#define UART_RX_ENABLE				0x00000001      // reciever enable
#define UART_TX_ENABLE				0x00000002      // transmitter enable
#define UART_INT_RX_ENABLED			0x00000004
#define UART_INT_TX_ENABLED			0x00000004

static UART_STRUCT * uart = NULL; //(UART_STRUCT *) UART_BASE;
static int irq;

static void irq_func_uart() {
	char ch = uart_getc();
}
AMBA_DEV dev;
int uart_init() {

	if (NULL != uart)
		return;
	TRY_CAPTURE_APB_DEV (&dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_UART);
	uart = (UART_STRUCT * )dev.bar[0].start;
	irq = dev.dev_info.irq;
	//disabled uart
	uart->ctrl = 0x0;
	uart->scaler = UART_SCALER_VAL;
	//enable uart
	uart->ctrl = /*UART_INT_RX_ENABLED |*/ UART_TX_ENABLE | UART_RX_ENABLE;
#ifndef SIMULATE
	while (!(UART_TX_READY & uart->status));
	//clear uart
	while (UART_RX_READY & uart->status);
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
// write character via uart
void uart_putc(char ch) {
	if (NULL == uart)
		uart_init();

#ifndef SIMULATE
	while (!(UART_TX_READY & uart->status))
		;
#endif
	uart->data = (UINT32)ch;
}

// read character via uart
char uart_getc() {
	if (NULL == uart)
		uart_init();

	while (!(UART_RX_READY & uart->status))
		;
	return ((char) uart->data);
}

int uart_set_irq_handler(IRQ_HANDLER pfunc)
{
	if (NULL == uart)
		uart_init();
	uart->ctrl |= UART_INT_RX_ENABLED ;
	irq_set_handler(irq, pfunc);
	return 0;
}

int uart_remove_irq_handler(IRQ_HANDLER pfunc)
{
	uart->ctrl &= ~UART_INT_RX_ENABLED ;
	irq_remove_handler(irq);
	return 0;
}
