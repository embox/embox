/**
 * @file
 * @brief This file implemented functions for work with xilix uart
 *        (for microblaze)
 *
 * @date 19.11.09
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <asm/bitops.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <module/embox/driver/serial/xuartlite.h>

EMBOX_UNIT_INIT(xuartlite_init);

#define UARTLITE_BASEADDR OPTION_GET(NUMBER,xuartlite_base)
#define UARTLITE_IRQ_NUM  OPTION_GET(NUMBER,irq_num)

/* There is no buad rate setup in QEMU, so it is unused. */
#define UARTLITE_BAUD_RATE 115200

struct xuartlite_regs {
	uint32_t rx_data;
	uint32_t tx_data;
	uint32_t status;
	uint32_t ctrl;
};

/* status registers bit definitions */
#define STATUS_PAR_ERROR             0x80
#define STATUS_FRAME_ERROR           0x40
#define STATUS_OVERUN_ERROR          0x20
#define STATUS_INTR_ENABLED          0x10
#define STATUS_TX_FIFO_FULL          0x08
#define STATUS_TX_FIFO_EMPTY         0x04
#define STATUS_RX_FIFO_FULL          0x02
#define STATUS_RX_FIFO_VALID_DATA    0x01

/* ctrl registers bit definitions */
#define CTRL_ENABLE_INTR             0x10
#define CTRL_RST_RX_FIFO             0x02
#define CTRL_RST_TX_FIFO             0x01

/*set registers base*/
static volatile struct xuartlite_regs *xuartlite_regs =
		(struct xuartlite_regs *) UARTLITE_BASEADDR;

static inline int xuartlite_is_rx_empty(void) {
	return !(xuartlite_regs->status & STATUS_RX_FIFO_VALID_DATA);
}

static inline int xuartlite_can_tx_trans(void) {
	return !(xuartlite_regs->status & STATUS_TX_FIFO_FULL);
}

static int xuartlite_setup(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		xuartlite_regs->ctrl |= CTRL_ENABLE_INTR;
	}

	return 0;
}

static int xuartlite_putc(struct uart *dev, int ch) {
	while (!xuartlite_can_tx_trans()) {
	}
	xuartlite_regs->tx_data = (unsigned int)ch;

	return 0;
}

static int xuartlite_getc(struct uart *dev) {
	return xuartlite_regs->rx_data & 0xFF;
}

static int xuartlite_has_symbol(struct uart *dev) {
	return !xuartlite_is_rx_empty();
}

static const struct uart_ops xuartlite_uart_ops = {
		.uart_getc = xuartlite_getc,
		.uart_putc = xuartlite_putc,
		.uart_hasrx = xuartlite_has_symbol,
		.uart_setup = xuartlite_setup,
};

static struct uart uart0 = {
		.uart_ops = &xuartlite_uart_ops,
		.irq_num = UARTLITE_IRQ_NUM,
		.base_addr = UARTLITE_BASEADDR,
};

static const struct uart_params uart_defparams = {
		.baud_rate = UARTLITE_BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = UARTLITE_BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

static int xuartlite_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

PERIPH_MEMORY_DEFINE(xuartlite, UARTLITE_BASEADDR, sizeof(struct xuartlite_regs));
