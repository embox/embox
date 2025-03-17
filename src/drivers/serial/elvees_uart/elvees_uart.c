/**
 * @file
 * @brief UART driver for ELVEES UART
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 
 * @date 14.11.2016
 */


#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>


#include <framework/mod/options.h>


#define UART(x)        (*(volatile uint32_t *)(dev->base_addr + (x)))

#define RBR  0x00
#define THR  0x00
#define DLL  0x00
#define DLH  0x04
#define IER  0x04
#define IIR  0x08
#define FCR  0x08
#define LCR  0x0C
#define MCR  0x10
#define LSR  0x14
#define MSR  0x18
#define SCR  0x1C
#define SRBR 0x30
#define STHR 0x30
#define USR  0x7C
#define TFL  0x80
#define RFL  0x84
#define SRR  0x88
#define SRTS 0x8C
#define SBCR 0x90
#define SFE  0x98
#define SRT  0x9C
#define STET 0xA0
#define HTX  0XA4

#define USR_BUSY         (1 << 0)
#define USR_TFNF         (1 << 1)

#define LSR_RDR          (1 << 0)
#define LSR_THRE         (1 << 5)
#define LCR_DLAB         (1 << 7)

#define IER_ERBFI        (1 << 0)

#define LCR_DLS_8BITS    (3 << 0)


#define PINS_INIT       OPTION_GET(NUMBER, pins_init)
#define USE_BOARD_CONF  OPTION_GET(BOOLEAN, use_bconf)

#if USE_BOARD_CONF
#include "uart_setup_hw_board_config.inc"
#else
static inline int uart_setup_hw(struct uart *dev) {
	return 0;
}

#endif /* USE_BOARD_CONF */

static inline void  elvees_uart_setup_baud_rate(struct uart *dev, int baudrate_bps) {
	uint32_t src_clock_hz = 48000000;
    uint32_t divisor = (uint32_t) (src_clock_hz / (baudrate_bps * 16));
    uint32_t tmp_lcr;

    tmp_lcr = UART(LCR);
    UART(LCR) = tmp_lcr | LCR_DLAB;

    UART(DLH) =  (divisor >> 8) & 0xFF;

    UART(DLL)= divisor & 0xFF;

    UART(LCR) = tmp_lcr;
}


static int elvees_uart_setup_common(struct uart *dev, const struct uart_params *params){

	uart_setup_hw(dev);
#if (PINS_INIT != 0)
	UART(SRR) = 1; /* soft reset */
	UART(FCR) = 0; /* without fifo */
	UART(LCR) = LCR_DLS_8BITS; /* 8 bit */

	elvees_uart_setup_baud_rate(dev, params->baud_rate);

	/*enable rx interrupt*/
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		/*enable rx interrupt*/
		UART(IER) = IER_ERBFI;
	}
#endif /*PINS_INIT  */
	return 0;
}


static int elvees_uart_has_symbol(struct uart *dev) {
	return UART(LSR) & LSR_RDR;
}

static int elvees_uart_getc(struct uart *dev) {
	return (char)UART(RBR);
}

static int elvees_uart_putc(struct uart *dev, int ch) {
	while (!(UART(LSR) & LSR_THRE)) {
	}

	UART(THR) = ch;

	return 0;
}

const struct uart_ops elvees_uart_uart_ops = {
	.uart_getc = elvees_uart_getc,
	.uart_putc = elvees_uart_putc,
	.uart_hasrx = elvees_uart_has_symbol,
	.uart_setup = elvees_uart_setup_common,
};
