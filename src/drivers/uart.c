#include "types.h"
#include "irq.h"
#include "leon_config.h"
#include "memory_map.h"
#include "uart.h"



typedef struct _UART_STRUCT
{
	volatile UINT32 data;		//0x70
	volatile UINT32 status;
	volatile UINT32 ctrl;
	volatile UINT32 scaler;
}UART_STRUCT;





#define UART_RX_READY           0x00000001      //
#define UART_TX_READY           0x00000004      // hold register empty


// control register bit masks
#define UART_DISABLE_ALL			0x00000000      // disable all

#define UART_RX_ENABLE				0x00000001      // reciever enable
#define UART_TX_ENABLE				0x00000002      // transmitter enable
#define UART_INT_RX_ENABLED			0x00000004
#define UART_INT_TX_ENABLED			0x00000004



UART_STRUCT *const uart = (UART_STRUCT *)UART_BASE;

static void irq_func_uart()
{
	char ch = uart_getc();
}
void uart_init ()
{
#ifndef RELEASE
	//disabled uart
	uart->ctrl = 0x0;
	uart->scaler = UART_SCALER_VAL;
	//enable uart
	uart->ctrl = UART_INT_RX_ENABLED | UART_TX_ENABLE | UART_RX_ENABLE ;
	while (!(UART_TX_READY & uart->status));
	//clear uart
	while (UART_RX_READY & uart->status)
		uart->data;
	//irq_set_handler(IRQ_UART1, irq_func_uart);
#endif
}

BOOL uart_is_empty ()
{
	if (UART_RX_READY & uart->status)
		return  FALSE;
	else return TRUE;
}
// write character via uart
void uart_putc(char ch)
{
	while (!(UART_TX_READY & uart->status)) ;
	uart->data = ch;
}

// read character via uart
char uart_getc()
{
	while (!(UART_RX_READY & uart->status)) ;
	return ((char)uart->data);
}




