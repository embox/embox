/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <asm/io.h>
#include <kernel/irq.h>
#include <stdio.h>
#include <util/ring_buff.h>
#include <drivers/serial.h>
#include <fs/file_desc.h>

#include <embox/unit.h>

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

/** Default I/O addresses
 * NOTE: The actual I/O addresses used are stored
 *       in a table in the BIOS data area 0000:0400.
 */
#define COM0_PORT           0x3f8
#define COM1_PORT           0x2f8
#define COM2_PORT           0x3e8
#define COM3_PORT           0x2e8

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
/* Divisor latch access bit */
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

static int uart_init(void) {
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

	return 0;
}

static void uart_putc(char ch) {
	while (!(in8(COM0_PORT + UART_LSR) & UART_EMPTY_TX));
	out8((uint8_t) ch, COM0_PORT + UART_TX);
}

static int uart_has_symbol(void) {
	return in8(COM0_PORT + UART_LSR) & UART_DATA_READY;
}

static char uart_getc(void) {
//	while (!uart_has_symbol());
	return in8(COM0_PORT + UART_RX);
}

static bool handler_was_set = false;
#define COM0_IRQ_NUM 0x4
#define UART_IER_RX_ENABLE 0x1

static int uart_set_irq_handler(irq_handler_t pfunc, struct file_desc *desc) {
	/*FIXME x86 uart 4 is number only for first port*/
	// TODO check return code.
	irq_attach(COM0_IRQ_NUM, pfunc,0,NULL,"uart");
	handler_was_set = true;
	/*enable rx interrupt*/
	out8(UART_IER_RX_ENABLE, COM0_PORT + UART_IER);
	return 0;
}
#if 1
static int uart_remove_irq_handler(void) {
	out8(UART_DLAB, COM0_PORT + UART_LCR);
	/*disable all uart interrupts*/
	out8(0x0, COM0_PORT + UART_IER);
	if (handler_was_set) {
		//irq_detach(COM0_IRQ_NUM, NULL);
		handler_was_set = false;
	}

	return 0;
}
#endif

#include <embox/device.h>
#include <fs/file.h>
#include <drivers/tty.h>

static tty_device_t tty;

static void *dev_open(struct file_desc *desc, const char *mode);
static int dev_close(struct file_desc *desc);
static size_t dev_read(void *buf, size_t size, size_t count, void *file);
static size_t dev_write(const void *buff, size_t size, size_t count, void *file);

static file_operations_t file_op = {
	.fread = dev_read,
	.fopen = dev_open,
	.fclose = dev_close,
	.fwrite = dev_write
};

RING_BUFFER_DEF(dev_buff, int, 0x20);

static irq_return_t irq_handler(irq_nr_t irq_nr, void *data) {
	int ch;
	if(uart_has_symbol()) {
		ch = uart_getc();
		ring_buff_enque(&dev_buff, &ch);
	}

	return 0;
}

/*
 * file_operation
 */
static void *dev_open(struct file_desc *desc, const char *mode) {
	uart_init();

	tty.file_op = &file_op;
	desc->ops = &file_op;
//	rc_pbuff = read_buff = rx_buff;
//	tty_register(&tty);
	uart_set_irq_handler(irq_handler, desc);
	return (void *) desc;
}

static int dev_close(struct file_desc *desc) {
//	tty_unregister(&tty);
	uart_remove_irq_handler();
	return 0;
}

static size_t dev_read(void *buff, size_t size, size_t count, void *file) {

	while(ring_buff_empty(&dev_buff)) {
	}

	for(;0 < count;--count) {
		ring_buff_deque(&dev_buff, buff);
		buff = (void *)(((uint32_t)buff) + size);
	}

	return 0;
}

static size_t dev_write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;

	while (cnt != count * size) {
		uart_putc(b[cnt++]);
	}
	return 0;
}

EMBOX_DEVICE("uart", &file_op);
