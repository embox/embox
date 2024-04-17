/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    20.02.2023
 */

#include <stdint.h>
#include <hal/reg.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>

#include <drivers/clk/k1921vg015_rcu.h>

#include <config/board_config.h>

#include <system_k1921vg015.h>

#include <framework/mod/options.h>

#define UARTCLK   OPTION_GET(NUMBER, uartclk)
#define BAUD_ICOEF(baud_rate) (UARTCLK / (16 * baud_rate))
#define BAUD_FCOEF(baud_rate) ((UARTCLK / (16.0f * baud_rate) - BAUD_ICOEF(baud_rate)) * 64 + 0.5f)

#if 0
struct uart_reg {
	uint32_t 	UART_DR_reg;
	uint32_t 	UART_RSR_reg;
	uint32_t reserved0[4];
	uint32_t 	UART_FR_reg;
	uint32_t reserved1[1];
	uint32_t 	UART_ILPR_reg;
	uint32_t 	UART_IBRD_reg;
	uint32_t 	UART_FBRD_reg;
	uint32_t 	UART_LCRH_reg;
	uint32_t 	UART_CR_reg;
	uint32_t 	UART_IFLS_reg;
	uint32_t 	UART_IMSC_reg;
	uint32_t 	UART_RIS_reg;
	uint32_t 	UART_MIS_reg;
	uint32_t 	UART_ICR_reg;
	uint32_t 	UART_DMACR_reg;
};
#endif

/* UART Registers */
#define UART_DR(base)   (base + 0x00)
#define UART_RSR(base)  (base + 0x04)
#define UART_ECR(base)  (base + 0x04)
#define UART_FR(base)   (base + 0x18)
#define UART_IBRD(base) (base + 0x24)
#define UART_FBRD(base) (base + 0x28)
#define UART_LCRH(base) (base + 0x2c)
#define UART_CR(base)   (base + 0x30)
#define UART_IMSC(base) (base + 0x38)
#define UART_MIS(base)  (base + 0x40)
#define UART_ICR(base)  (base + 0x44)

#define UART_FEN        (1 << 4)
#define UART_UARTEN     (1 << 0)
#define UART_TXE        (1 << 8)
#define UART_RXE        (1 << 9)
#define UART_WLEN_8BIT  0x3
#define UART_WLEN_SHIFT 5

/* Flag register */
#define FR_RXFE         0x10 /* Receive FIFO empty */
#define FR_TXFF         0x20 /* Transmit FIFO full */

#define IMSC_RXIM       (0x1 << 4)

#define UART_GPIO_PORT         GPIO_PORT_A
#define UART_GPIO_TX_PIN		(1)
#define UART_GPIO_RX_PIN		(0)

static inline int niiet_uart_nr_by_addr(uintptr_t base_addr) {
	int num;

	switch (base_addr) {
#if defined CONF_UART0_ENABLED
	case CONF_UART0_REGION_BASE_ADDR:
		num = 0;
		break;
#endif
#if defined CONF_UART1_ENABLED
	case CONF_UART1_REGION_BASE_ADDR:
		num = 1;
		break;
#endif
#if defined CONF_UART2_ENABLED
	case CONF_UART2_REGION_BASE_ADDR:
		num = 2;
		break;
#endif
#if defined CONF_UART3_ENABLED
	case CONF_UART3_REGION_BASE_ADDR:
		num = 3;
		break;
#endif
#if defined CONF_UART4_ENABLED
	case CONF_UART4_REGION_BASE_ADDR:
		num = 4;
		break;
#endif
	default:
		return -1;
	}
	return num;
}

static inline int niiet_uart_get_tx_port(int num) {
	int port = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_PORT
	case 0:
		port = CONF_UART0_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART1_PIN_TX_PORT
	case 1:
		port = CONF_UART1_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART2_PIN_TX_PORT
	case 2:
		port = CONF_UART2_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART3_PIN_TX_PORT
	case 3:
		port = CONF_UART3_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART4_PIN_TX_PORT
	case 4:
		port = CONF_UART4_PIN_TX_PORT;
		break;
#endif
	default:
		port = -1;
	}

	return port;
}

static inline int niiet_uart_get_tx_pin(int num) {
	int pin = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_NR
	case 0:
		pin = CONF_UART0_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART1_PIN_TX_NR
	case 1:
		pin = CONF_UART1_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART2_PIN_TX_NR
	case 2:
		pin = CONF_UART2_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART3_PIN_TX_NR
	case 3:
		pin = CONF_UART3_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART4_PIN_TX_NR
	case 4:
		pin = CONF_UART4_PIN_TX_NR;
		break;
#endif
	default:
		pin = -1;
	}

	return pin;
}

static inline int niiet_uart_get_tx_alt(int num) {
	int alt = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_AF
	case 0:
		alt = CONF_UART0_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART1_PIN_TX_AF
	case 1:
		alt = CONF_UART1_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART2_PIN_TX_AF
	case 2:
		alt = CONF_UART2_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART3_PIN_TX_AF
	case 3:
		alt = CONF_UART3_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART4_PIN_TX_NR
	case 4:
		alt = CONF_UART4_PIN_TX_NR;
		break;
#endif
	default:
		alt = -1;
	}

	return alt;
}

static inline int niiet_uart_get_rx_port(int num) {
	int port = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_PORT
	case 0:
		port = CONF_UART0_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART1_PIN_RX_PORT
	case 1:
		port = CONF_UART1_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART2_PIN_RX_PORT
	case 2:
		port = CONF_UART2_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART3_PIN_RX_PORT
	case 3:
		port = CONF_UART3_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART4_PIN_RX_PORT
	case 4:
		port = CONF_UART4_PIN_RX_PORT;
		break;
#endif
	default:
		port = -1;
	}

	return port;
}

static inline int niiet_uart_get_rx_pin(int num) {
	int pin = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_NR
	case 0:
		pin = CONF_UART0_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART1_PIN_RX_NR
	case 1:
		pin = CONF_UART1_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART2_PIN_RX_NR
	case 2:
		pin = CONF_UART2_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART3_PIN_RX_NR
	case 3:
		pin = CONF_UART3_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART4_PIN_RX_NR
	case 4:
		pin = CONF_UART4_PIN_RX_NR;
		break;
#endif
	default:
		pin = -1;
	}

	return pin;
}

static inline int niiet_uart_get_rx_alt(int num) {
	int alt = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_AF
	case 0:
		alt = CONF_UART0_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART1_PIN_RX_AF
	case 1:
		alt = CONF_UART1_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART2_PIN_RX_AF
	case 2:
		alt = CONF_UART2_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART3_PIN_RX_AF
	case 3:
		alt = CONF_UART3_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART4_PIN_RX_NR
	case 4:
		alt = CONF_UART4_PIN_RX_NR;
		break;
#endif
	default:
		alt = -1;
	}

	return alt;
}

static inline void niiet_uart_set_pins(int num) {
	gpio_setup_mode(niiet_uart_get_tx_port(num),
			(1 << niiet_uart_get_tx_pin(num)),
			GPIO_MODE_OUT_ALTERNATE |
			GPIO_ALTERNATE(niiet_uart_get_tx_alt(num)));

	gpio_setup_mode(niiet_uart_get_rx_port(num),
			(1 << niiet_uart_get_rx_pin(num)),
			GPIO_MODE_OUT_ALTERNATE |
			GPIO_ALTERNATE(niiet_uart_get_tx_alt(num)));
}

static inline int niiet_uart_set_clk(int num) {
	char *clk_name;
	switch (num) {
#if defined CONF_UART0_CLK_ENABLE
		case 0:
		clk_name = CONF_UART0_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART1_CLK_ENABLE
		case 1:
		clk_name = CONF_UART1_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART2_CLK_ENABLE
		case 2:
		clk_name = CONF_UART2_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART3_CLK_ENABLE
		case 3:
		clk_name = CONF_UART3_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART4_CLK_ENABLE
		case 4:
		clk_name = CONF_UART4_CLK_ENABLE();
		break;
#endif
	default:
		return -1;
	}

	clk_enable(clk_name);

	return  0;
}

static void niiet_uart_set_baudrate(struct uart *dev) {
	/* FIXME Init baud rate only if UARTCLK is really used.
	 * Currenly it is not so for the teplates which use pl011. */
#if UARTCLK != 0
	uint32_t baud_rate;
	int ibrd, fbrd;

	/* Baud Rate Divisor = UARTCLK/(16×Baud Rate) = BRDI + BRDF,
	 * See 2.4.3 UART operation.  */
	baud_rate = dev->params.baud_rate;
	ibrd = BAUD_ICOEF(baud_rate);
	fbrd = BAUD_FCOEF(baud_rate);
	REG32_STORE(UART_IBRD(dev->base_addr), ibrd);
	REG32_STORE(UART_FBRD(dev->base_addr), fbrd);
#endif
}

static int niiet_uart_setup(struct uart *dev, const struct uart_params *params) {
	int uart_num = niiet_uart_nr_by_addr(dev->base_addr);

	/* Disable uart. */
	REG32_STORE(UART_CR(dev->base_addr), 0);

	niiet_uart_set_pins(uart_num);
	niiet_uart_set_clk(uart_num);

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	}

	niiet_uart_set_baudrate(dev);

	/* Word len 8 bit. */
	REG32_STORE(UART_LCRH(dev->base_addr), UART_WLEN_8BIT << UART_WLEN_SHIFT);

	/* Enable uart. */
	REG32_STORE(UART_CR(dev->base_addr), UART_UARTEN | UART_TXE | UART_RXE);

	return 0;
}

static int niiet_uart_irq_enable(struct uart *dev,
    const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	}
	return 0;
}

static int niiet_uart_irq_disable(struct uart *dev,
    const struct uart_params *params) {
	REG32_STORE(UART_IMSC(dev->base_addr), 0);

	return 0;
}

static int niiet_uart_putc(struct uart *dev, int ch) {
	while (REG32_LOAD(UART_FR(dev->base_addr)) & FR_TXFF) {
		;
	}

	REG32_STORE(UART_DR(dev->base_addr), (uint32_t)ch);

	return 0;
}

static int niiet_uart_hasrx(struct uart *dev) {
	return !(REG32_LOAD(UART_FR(dev->base_addr)) & FR_RXFE);
}

static int niiet_uart_getc(struct uart *dev) {
	return REG32_LOAD(UART_DR(dev->base_addr));
}

const struct uart_ops niiet_uart_ops = {
		.uart_setup = niiet_uart_setup,
		.uart_putc  = niiet_uart_putc,
		.uart_hasrx = niiet_uart_hasrx,
		.uart_getc  = niiet_uart_getc,
		.uart_irq_en = niiet_uart_irq_enable,
		.uart_irq_dis = niiet_uart_irq_disable,
};


