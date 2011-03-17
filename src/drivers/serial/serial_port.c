/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */
#include <types.h>
#include <kernel/diag.h>
#include <asm/io.h>

/**
 * UART registers
 *           +---------------------+---------------------+
 *           |      DLAB = 0       |       DLAB = 1      |
 * +---------+---------+-----------+---------+-----------+
 * |I/O port |  Read   |  Write    | Read    |Write      |
 * +-------------------+-----------+---------+-----------+
 * |base + 0 |  RBR    |  THR      |                     |
 * |         |receiver |transmitter|DLL divisor latch LSB|
 * |         | buffer  | holding   |                     |
 * +---------+---------+-----------+---------------------+
 * |base + 1 |  IER    |   IER     |                     |
 * |         |interrupt|interrupt  |DLM divisor latch MSB|
 * |         | enable  | enable    |                     |
 * +---------+---------+-----------+---------+-----------+
 * |base + 2 |  IIR    |  FCR      |  IIR    |  FCR      |
 * |         |interrupt|  FIFO     |interrupt|  FIFO     |
 * |         |identif. | control   |identif. | control   |
 * +---------+---------+-----------+---------+-----------+
 * |base + 3 |            LCR line control               |
 * +---------+-------------------------------------------+
 * |base + 4 |            MCR modem control              |
 * +---------+---------+-----------+---------------------+
 * |base + 5 |  LSR    |    -      |  LSR    |    -      |
 * |         |  line   | factory   |  line   | factory   |
 * |         | status  |   test    | status  |   test    |
 * +---------+---------+-----------+---------------------+
 * |base + 6 |  MSR    |    -      |  MSR    |    -      |
 * |         |  modem  |   not     |  modem  |   not     |
 * |         | status  |  used     | status  |  used     |
 * +---------+---------+-----------+---------------------+
 * |base + 7 |            SCR scratch                    |
 * +---------+---------+-----------+---------------------+
 */

#define COM_PORT            0x3f8

/* The offsets of UART registers */
#define UART_TX             0x0
#define UART_RX             0x0
#define UART_DLL            0x0
#define UART_IER            0x1
#define UART_DLH            0x1
#define UART_IIR            0x2
#define UART_FCR            0x2
#define UART_LCR            0x3
#define UART_MCR            0x4
#define UART_LSR            0x5
#define UART_MSR            0x6
#define UART_SR             0x7

#define UART_DATA_READY     0x01
#define UART_EMPTY_TX       0x20
#define UART_ENABLE_FIFO    0xC7
#define UART_ENABLE_MODEM   0x0B
#define UART_DLAB           0x80

/* The type of word length */
#define UART_5BITS_WORD     0x00
#define UART_6BITS_WORD     0x01
#define UART_7BITS_WORD     0x02
#define UART_8BITS_WORD     0x03

/* The type of parity */
#define UART_NO_PARITY      0x00
#define UART_ODD_PARITY     0x08
#define UART_EVEN_PARITY    0x18

/* The type of the length of stop bit */
#define UART_1_STOP_BIT     0x00
#define UART_2_STOP_BITS    0x04

#define DIVISOR(baud) (115200 / baud)

static bool serial_inited = 0;

int uart_init(void) {
	diag_init();
	return 0;
}

void uart_putc(char ch) {
	diag_putc(ch);
}

char uart_getc(void) {
	return diag_getc();
}

void diag_init(void) {
	if (serial_inited) {
		return;
	}
	/* Turn off the interrupt */
	out8(COM_PORT + UART_IER, 0x0);
	/* Set DLAB */
	out8(COM_PORT + UART_LCR, UART_DLAB);
	/* Set the baud rate */
	out8(COM_PORT + UART_DLL, DIVISOR(CONFIG_UART_BAUD_RATE) & 0xFF);
	out8(COM_PORT + UART_DLH, (DIVISOR(CONFIG_UART_BAUD_RATE) >> 8) & 0xFF);
	/* Set the line status */
	out8(COM_PORT + UART_LCR,
		UART_NO_PARITY | UART_8BITS_WORD | UART_1_STOP_BIT);
	/* Uart enable FIFO */
	out8(COM_PORT + UART_FCR, UART_ENABLE_FIFO);
	/* Uart enable modem (turn on DTR, RTS, and OUT2) */
	out8(COM_PORT + UART_MCR, UART_ENABLE_MODEM);
	serial_inited = 1;
}

int diag_has_symbol(void) {
	if (!serial_inited) {
		return EOF;
	}
	return in8(COM_PORT + UART_LSR) & UART_DATA_READY;
}

char diag_getc(void) {
	while (!diag_has_symbol());
	return in8(COM_PORT + UART_RX);
}

void diag_putc(char ch) {
	while (!(in8(COM_PORT + UART_LSR) & UART_EMPTY_TX));
	out8(COM_PORT + UART_TX, (uint8_t) ch);
}

