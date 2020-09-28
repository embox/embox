/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    25.07.2020
 */

#include <stdint.h>
//#include <string.h>

#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>

#include <drivers/ttys.h>
//#include <framework/mod/options.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

#define BAUD_RATE 115200
//#define BAUD_RATE OPTION_GET(NUMBER,baud_rate)

static struct usb_gadget_ep *bulk_tx;
static struct usb_gadget_ep *bulk_rx;

static struct usb_gadget_request req_tx;
static struct usb_gadget_request req_rx;

static int acm_uart_putc(struct uart *dev, int ch) {
	usb_gadget_ep_queue(bulk_tx, &req_tx);
	return 0;
}

static int acm_uart_hasrx(struct uart *dev) {
	return 0;
}

static int acm_uart_getc(struct uart *dev) {
	return 0;
}

static int acm_uart_setup(struct uart *dev, const struct uart_params *params) {
	return 0;
}

static int acm_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	return 0;
}

static int acm_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	return 0;
}

const struct uart_ops acm_uart_ops = {
		.uart_getc = acm_uart_getc,
		.uart_putc = acm_uart_putc,
		.uart_hasrx = acm_uart_hasrx,
		.uart_setup = acm_uart_setup,
		.uart_irq_en = acm_uart_irq_en,
		.uart_irq_dis = acm_uart_irq_dis,
};

static struct uart acm_diag = {
		.uart_ops = &acm_uart_ops,
		//.irq_num = 71, //USART6_IRQn
		//.base_addr = (unsigned long) 0x40011400UL,//USARTx
};

static const struct uart_params diag_defparams = {
		.baud_rate = BAUD_RATE,
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

DIAG_SERIAL_DEF(&acm_diag, &diag_defparams);

static struct uart acm_ttyS0 = {
		.uart_ops = &acm_uart_ops,
		//.irq_num = IRQ_NUM,
		//.base_addr = (unsigned long) UART_BASE,
		.params = {
				.baud_rate = BAUD_RATE,
				.parity = 0,
				.n_stop = 1,
				.n_bits = 8,
				.irq = true,
		}
};

TTYS_DEF("ttyACM0", &acm_ttyS0);

int acm_rx_complete(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	return 0;
}

int acm_tty_init(struct usb_gadget_ep *tx, struct usb_gadget_ep *rx) {
	bulk_tx = tx;
	bulk_rx = rx;

	//initialize request structs
	req_rx.complete = acm_rx_complete;
	req_rx.len = sizeof req_rx.buf;
	usb_gadget_ep_queue(bulk_rx, &req_rx);

	req_tx.len = sizeof req_tx.buf;
	return 0;
}
