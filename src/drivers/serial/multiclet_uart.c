/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.07.2015
 */


 /**
   ******************************************************************************
   *  _ _ _    _ _ _   _ _
   * |     \  /     |/     \
   * |      \/           _ _\
   * |                  /
   * |         MultiClet
   * |                  \ _ _
   * |                      /
   * |_ _ _|\/|_ _ _|\ _ _ /
   *
   * @file    uart.c
   * @author  multiclet dev team
   * @date    13-07-2015
   * @brief   UART example.
   *
   ******************************************************************************
*/

#include <compiler.h>
#include <drivers/diag.h>

#include <framework/mod/options.h>
#include <module/embox/driver/serial/multiclet.h>

#include <uart.h>

static void mcp_putc(const struct diag *dev, char ch) {
	//UART_SEND_BYTE(ch, UART0);
	DM2UART(UART0, (int)&ch, 1);
}

static int mcp_init(const struct diag *dev) {
	UART_InitTypeDef UART_InitStructure;

	UART_InitStructure.System_freq = 8000000;
	UART_InitStructure.BaudRate = OPTION_GET(NUMBER, baud_rate);
	UART_InitStructure.TypeParity = 0x00000000;
	UART_InitStructure.Parity = 0x00000000;
	UART_InitStructure.FlowControl = 0x00000000;
	UART_InitStructure.Mode = 0x00000003;

	GPIOC->BPS = (1<<10)|(1<<11);
	uart_init(UART0, &UART_InitStructure);
	return 0;
}

DIAG_OPS_DEF(
	.init = mcp_init,
	.putc = mcp_putc,
);
