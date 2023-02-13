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


#define UART_BASE      OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM        OPTION_GET(NUMBER, irq_num)

#define PINS_INIT      OPTION_GET(NUMBER, pins_init)

#define UART(x)        (*(volatile uint32_t *)(UART_BASE + (x)))

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

#define USR_BUSY  (1 << 0)
#define USR_TFNF  (1 << 1)
#define LSR_RDR   (1 << 0)
#define LSR_THRE  (1 << 5)
#define LCR_DLAB  (1 << 7)

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

#if PINS_INIT == 0

static int elvees_uart_setup(struct uart *dev, const struct uart_params *params) {
	return 0;
}

#else

#include <drivers/gpio/gpio_driver.h>
static int elvees_uart_setup(struct uart *dev, const struct uart_params *params) {
#define GPIO_ALT_FUNC_UART   (4)

	gpio_setup_mode(3, 1 << 0, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(GPIO_ALT_FUNC_UART));
	gpio_setup_mode(3, 1 << 1, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(GPIO_ALT_FUNC_UART));

	UART(SRR) = 1; /* soft reset */
	UART(FCR) = 0; /* without fifo */
	UART(LCR) = 3; /* 8 bit */

	elvees_uart_setup_baud_rate(dev, 115200);

	/*enable rx interrupt*/
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		/*enable rx interrupt*/
	}

	return 0;
}
#endif

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
		.uart_setup = elvees_uart_setup,
};
