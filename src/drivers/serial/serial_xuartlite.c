/**
 * @file
 * @brief This file implemented functions for work with xilix uart
 *        (for microblaze)
 *
 * @date 19.11.09
 * @author Anton Bondarev
 */

#include <types.h>
#include <bitops.h>
#include <asm/cpu_conf.h>
#include <kernel/irq.h>

typedef struct uart_regs {
	uint32_t rx_data;
	uint32_t tx_data;
	uint32_t status;
	uint32_t ctrl;
} uart_regs_t;

/*status registers bit definitions*/
#define STATUS_PAR_ERROR_BIT          24
#define STATUS_FRAME_ERROR_BIT        25
#define STATUS_OVERUN_ERROR_BIT       26
#define STATUS_INTR_ENABLED_BIT       27
#define STATUS_TX_FIFO_FULL_BIT       28
#define STATUS_TX_FIFO_EMPTY_BIT      29
#define STATUS_RX_FIFO_FULL_BIT       30
#define STATUS_RX_FIFO_VALID_DATA_BIT 31
/*ctrl registers bit definitions*/
#define CTRL_ENABLE_INTR_BIT          27
#define CTRL_RST_RX_FIFO_BIT          30
#define CTRL_RST_TX_FIFO_BIT          31

#define STATUS_PAR_ERROR             REVERSE_MASK(STATUS_PAR_ERROR_BIT)
#define STATUS_FRAME_ERROR           REVERSE_MASK(STATUS_FRAME_ERROR_BIT)
#define STATUS_OVERUN_ERROR          REVERSE_MASK(STATUS_OVERUN_ERROR_BIT)
#define STATUS_INTR_ENABLED          REVERSE_MASK(STATUS_INTR_ENABLED_BIT)
#define STATUS_TX_FIFO_FULL          REVERSE_MASK(STATUS_TX_FIFO_FULL_BIT)
#define STATUS_TX_FIFO_EMPTY         REVERSE_MASK(STATUS_TX_FIFO_EMPTY_BIT)
#define STATUS_RX_FIFO_FULL          REVERSE_MASK(STATUS_RX_FIFO_FULL_BIT)
#define STATUS_RX_FIFO_VALID_DATA    REVERSE_MASK(STATUS_RX_FIFO_VALID_DATA_BIT)

#define CTRL_ENABLE_INTR             REVERSE_MASK(CTRL_ENABLE_INTR_BIT)
#define CTRL_RST_RX_FIFO             REVERSE_MASK(CTRL_RST_RX_FIFO_BIT)
#define CTRL_RST_TX_FIFO             REVERSE_MASK(CTRL_RST_TX_FIFO_BIT)

/*set registers base*/
static volatile uart_regs_t *uart = (uart_regs_t *) XILINX_UARTLITE_BASEADDR;

static inline int is_rx_empty(void) {
	return !(uart->status & STATUS_RX_FIFO_VALID_DATA);
}

static inline int can_tx_trans(void) {
	return !(uart->status & STATUS_TX_FIFO_FULL);
}

int uart_init(void) {
	return 0;
}

char uart_getc(void) {
	while (is_rx_empty());
	return (char) (uart->rx_data & 0xFF);
}

void uart_putc(char ch) {
	while (!can_tx_trans());
	uart->tx_data = (unsigned int)ch;
}

/*
 * diag interface
 */
int diag_init(void) {
	return uart_init();
}

char diag_getc(void) {
	return uart_getc();
}

void diag_putc(char ch) {
	uart_putc(ch);
}

/* TODO uart_set_irq_handler haven't to be used*/
int uart_set_irq_handler(irq_handler_t pfunc) {
	irq_attach(XILINX_UARTLITE_IRQ_NUM, pfunc, 0, "xil_uartlite", NULL);
	return 0;
}

int uart_remove_irq_handler(void) {
	return 0;
}
