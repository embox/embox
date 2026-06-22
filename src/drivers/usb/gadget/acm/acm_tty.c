/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    25.07.2020
 */

#include <stdint.h>
#include <string.h>

#include <hal/ipl.h>
#include <kernel/irq.h>

#include <drivers/serial/uart_dev.h>
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

#define ACM_RX_RING_SZ 256
static uint8_t acm_rx_buf[ACM_RX_RING_SZ];
static unsigned int acm_rx_head;
static unsigned int acm_rx_tail;

#define ACM_TX_RING_SZ 256
static uint8_t acm_tx_buf[ACM_TX_RING_SZ];
static unsigned int acm_tx_head;
static unsigned int acm_tx_tail;
static int acm_tx_busy;

static void acm_tx_kick(void) {
	unsigned int len = 0;

	if (acm_tx_busy || acm_tx_head == acm_tx_tail) {
		return;
	}

	while (acm_tx_tail != acm_tx_head && len < sizeof(req_tx.buf)) {
		req_tx.buf[len++] = acm_tx_buf[acm_tx_tail];
		acm_tx_tail = (acm_tx_tail + 1) % ACM_TX_RING_SZ;
	}

	req_tx.len = len;
	acm_tx_busy = 1;
	usb_gadget_ep_queue(bulk_tx, &req_tx);
}

int acm_tx_complete(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	ipl_t ipl;

	ipl = ipl_save();
	{
		acm_tx_busy = 0;
		acm_tx_kick();
	}
	ipl_restore(ipl);

	return 0;
}

static int acm_uart_putc(struct uart *dev, int ch) {
	ipl_t ipl;
	unsigned int next_head;

	if (bulk_tx == NULL) {
		return 0;
	}

	ipl = ipl_save();
	{
		next_head = (acm_tx_head + 1) % ACM_TX_RING_SZ;
		if (next_head != acm_tx_tail) {
			acm_tx_buf[acm_tx_head] = (uint8_t)ch;
			acm_tx_head = next_head;
		}

		acm_tx_kick();
	}
	ipl_restore(ipl);

	return 0;
}

static int acm_uart_hasrx(struct uart *dev) {
	return acm_rx_head != acm_rx_tail;
}

static int acm_uart_getc(struct uart *dev) {
	int ch;

	if (acm_rx_head == acm_rx_tail) {
		return 0;
	}

	ch = acm_rx_buf[acm_rx_tail];
	acm_rx_tail = (acm_rx_tail + 1) % ACM_RX_RING_SZ;

	return ch;
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
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&acm_diag, &diag_defparams);

#include <util/macro.h>

#define TTY_NAME   ttyACM0

static struct uart acm_ttyS0 = {
		.uart_ops = &acm_uart_ops,
		//.irq_num = IRQ_NUM,
		//.base_addr = (unsigned long) UART_BASE,
		.params = {
				.baud_rate = BAUD_RATE,
				.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
		}
};

TTYS_DEF(TTY_NAME, &acm_ttyS0);

int acm_rx_complete(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	unsigned int i;

	for (i = 0; i < req->actual_len; i++) {
		unsigned int next_head = (acm_rx_head + 1) % ACM_RX_RING_SZ;

		if (next_head == acm_rx_tail) {
			break;
		}

		acm_rx_buf[acm_rx_head] = req->buf[i];
		acm_rx_head = next_head;
	}

	uart_irq_handler(0, &acm_ttyS0);

	req_rx.len = sizeof(req_rx.buf);
	usb_gadget_ep_queue(bulk_rx, &req_rx);
	return 0;
}

int acm_tty_init(struct usb_gadget_ep *tx, struct usb_gadget_ep *rx) {
	bulk_tx = tx;
	bulk_rx = rx;

	acm_rx_head = 0;
	acm_rx_tail = 0;

	acm_tx_head = 0;
	acm_tx_tail = 0;
	acm_tx_busy = 0;

	//initialize request structs
	req_rx.complete = acm_rx_complete;
	req_rx.len = sizeof req_rx.buf;
	usb_gadget_ep_queue(bulk_rx, &req_rx);

	req_tx.complete = acm_tx_complete;
	req_tx.len = sizeof req_tx.buf;
	return 0;
}
