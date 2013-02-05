/**
 * @file
 * @brief diag interface for 8250/16550-type serial ports
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <drivers/diag.h>
#include <asm/io.h>
#include <drivers/serial/8250.h>

#include <embox/unit.h>


/** Default I/O addresses
 * NOTE: The actual I/O addresses used are stored
 *       in a table in the BIOS data area 0000:0400.
 */
#define COM0_PORT           0x3f8
#define COM1_PORT           0x2f8
#define COM2_PORT           0x3e8
#define COM3_PORT           0x2e8



void diag_init(void) {
	/* Turn off the interrupt */
	out8(0x0, COM0_PORT + UART_IER);
	/* Set DLAB */
	out8(UART_DLAB, COM0_PORT + UART_LCR);
	/* Set the baud rate */
	out8(DIVISOR(OPTION_GET(NUMBER, baud_rate)) & 0xFF, COM0_PORT + UART_DLL);
	out8((DIVISOR(OPTION_GET(NUMBER, baud_rate)) >> 8) & 0xFF, COM0_PORT + UART_DLH);
	/* Set the line status */
	out8(UART_NO_PARITY | UART_8BITS_WORD | UART_1_STOP_BIT, COM0_PORT + UART_LCR);
	/* Uart enable FIFO */
	out8(UART_ENABLE_FIFO, COM0_PORT + UART_FCR);
	/* Uart enable modem (turn on DTR, RTS, and OUT2) */
	out8(UART_ENABLE_MODEM, COM0_PORT + UART_MCR);
}

char diag_getc(void) {
	while (!diag_has_symbol());
	return in8(COM0_PORT + UART_RX);
}

void diag_putc(char ch) {
	while (!(in8(COM0_PORT + UART_LSR) & UART_EMPTY_TX));
	out8((uint8_t) ch, COM0_PORT + UART_TX);
}

int diag_has_symbol(void) {
	return in8(COM0_PORT + UART_LSR) & UART_DATA_READY;
}
