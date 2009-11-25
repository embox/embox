/**
 * @file serial_xuartlite.c
 * @brief This file implemented functions for work with xilix uart (for microblaze)
 * @date 19.11.2009
 * @author: Anton Bondarev
 */

#include "autoconf.h"
#include "kernel/irq.h"

typedef struct uart_regs {
	unsigned int rx_data;
	unsigned int tx_data;
	unsigned int status;
}uart_regs_t;

#define SR_TX_FIFO_FULL	       0x08 /**< transmit FIFO full */
#define SR_RX_FIFO_VALID_DATA  0x01 /**< data in receive FIFO */
#define SR_RX_FIFO_FULL        0x02 /**< receive FIFO full */

int uart_init() {

}

char uart_getc() {
}

void uart_putc(char ch){

}

int uart_set_irq_handler(IRQ_HANDLER pfunc) {

	return 0;
}

int uart_remove_irq_handler() {

	return 0;
}
