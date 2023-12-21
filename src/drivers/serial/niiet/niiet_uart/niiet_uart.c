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

#include <system_k1921vg015.h>

#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)

#define UARTCLK   OPTION_GET(NUMBER, uartclk)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

#define BAUD_ICOEF (UARTCLK / (16 * BAUD_RATE))
#define BAUD_FCOEF ((UARTCLK / (16.0f * BAUD_RATE) - BAUD_ICOEF) * 64 + 0.5f)
#if 0
#define RCU				0x40041000

#define RCU_SYSCLKCFG			0x40041010
#define RCU_SYSCLKCFG_SYSSEL_OSICLK	0x00000000
#define RCU_SYSCLKCFG_SYSSEL_OSECLK	0x00000001
#define RCU_SYSCLKCFG_SYSSEL_PLLCLK	0x00000002
#define RCU_SYSCLKCFG_SYSSEL_PLLDIVCLK	0x00000003

#define RCU_SYSCLKSTAT			0x40041014
#define RCU_SYSCLKSTAT_SYSSTAT_mask	0x00000003

#define RCU_UARTCFG			0x40041060
#define RCU_UART0CFG			0x40041060
#define RCU_UART1CFG			0x40041064
#define RCU_UARTCFG_CLKEN_mask		0x00000001
#define RCU_UARTCFG_RSTDIS_mask		0x00000010
#define RCU_UARTCFG_CLKSEL_OSECLK	0x00000000
#define RCU_UARTCFG_CLKSEL_PLLCLK	0x00000100
#define RCU_UARTCFG_CLKSEL_PLLDIVCLK	0x00000200
#define RCU_UARTCFG_CLKSEL_OSICLK	0x00000300
#endif
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

static void niiet_uart_set_baudrate(struct uart *dev) {
	/* FIXME Init baud rate only if UARTCLK is really used.
	 * Currenly it is not so for the teplates which use pl011. */
#if UARTCLK != 0
	uint32_t baud_rate;
	int ibrd, fbrd;

	/* Baud Rate Divisor = UARTCLK/(16×Baud Rate) = BRDI + BRDF,
	 * See 2.4.3 UART operation.  */
	baud_rate = dev->params.baud_rate;
	ibrd = (UARTCLK / (16 * baud_rate));
	fbrd = ((UARTCLK % (16 * baud_rate)) * 64) / (16 * baud_rate);
	REG32_STORE(UART_IBRD(dev->base_addr), ibrd);
	REG32_STORE(UART_FBRD(dev->base_addr), fbrd);
#endif
}

#define UART_GPIO_PORT         GPIO_PORT_A
#define UART_GPIO_TX_PIN		(1 << 0)
#define UART_GPIO_RX_PIN		(1 << 1)

static inline int niiet_uart_nr_by_addr(uintptr_t base_addr) {
	return 0;
}

static inline void niiet_uart_set_rcu(struct uart *dev) {
	int uart_num = niiet_uart_nr_by_addr(dev->base_addr);
	
	RCU->RCU_CGCFGAPB_reg |= RCU_CGCFGAPB_UART_EN(uart_num);
	RCU->RCU_RSTDISAPB_reg |= RCU_RSTDISAPB_UART_EN(uart_num);

	RCU->RCU_UARTCLKCFG0_reg &= ~(RCU_CLKSTAT_SRC_MASK);
	RCU->RCU_UARTCLKCFG0_reg |= (RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) ;

	RCU->RCU_UARTCLKCFG0_reg |= RCU_UARTCLKCFG0_CLKEN_MASK;
    RCU->RCU_UARTCLKCFG0_reg |= RCU_UARTCLKCFG0_RSTDIS_MASK;
}

static inline void niiet_uart_set_pins(struct uart *dev) {
	gpio_setup_mode(UART_GPIO_PORT,
			(1 << UART_GPIO_TX_PIN) | (1 << UART_GPIO_RX_PIN),
			 GPIO_MODE_OUT_ALTERNATE);
}

static int niiet_uart_setup(struct uart *dev, const struct uart_params *params) {

	/* Disable uart. */
	REG32_STORE(UART_CR(dev->base_addr), 0);

	niiet_uart_set_pins(dev);
	niiet_uart_set_rcu(dev);

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	}

	niiet_uart_set_baudrate(dev);

	/* Word len 8 bit. */
	REG32_STORE(UART_LCRH(dev->base_addr), UART_WLEN_8BIT << UART_WLEN_SHIFT);

	/* Enable uart. */
	REG32_STORE(UART_CR(dev->base_addr), UART_UARTEN | UART_TXE | UART_RXE);

	return 0;
#if 0
	/* Disable uart. */
	REG_STORE(UART0_CR, 0);
	/* Enable uart0 pins */
	gpio_setup_mode(UART0_GPIO, UART0_GPIO_TX_mask | UART0_GPIO_RX_mask, GPIO_MODE_OUT_ALTERNATE);
	/* Clock for uart0 */
	REG32_STORE(RCU_UART0CFG, RCU_UARTCFG_CLKEN_mask | RCU_UARTCFG_RSTDIS_mask | RCU_UARTCFG_CLKSEL_PLLCLK);
	/* Set baud rate */
	REG32_STORE(UART0_IBRD, BAUD_ICOEF);
	REG32_STORE(UART0_FBRD, BAUD_FCOEF);
	/* FIFO enable, 8 data bits */
	REG32_STORE(UART0_LCRH, UART_LCRH_FEN_mask | UART_LCRH_WLEN_8bit);
	/* Enable UART */
	REG32_STORE(UART0_CR, UART_CR_TXE_mask | UART_CR_RXE_mask | UART_CR_UARTEN_mask);

	return 0;
#endif
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
#if 0
	while (REG32_LOAD(UART0_FR) & UART_FR_BUSY_mask);
	REG32_STORE(UART0_DR, (uint32_t) ch);
#endif

	while (REG32_LOAD(UART_FR(dev->base_addr)) & FR_TXFF) {
		;
	}

	REG32_STORE(UART_DR(dev->base_addr), (uint32_t)ch);

	return 0;
}

static int niiet_uart_hasrx(struct uart *dev) {
	return !(REG32_LOAD(UART_FR(dev->base_addr)) & FR_RXFE);

#if 0
	return !(REG32_LOAD(UART0_FR) & UART_FR_RXFE_mask);
#endif
}

static int niiet_uart_getc(struct uart *dev) {
	return REG32_LOAD(UART_DR(dev->base_addr));

#if 0
	return (int) (REG32_LOAD(UART0_DR) & UART_DR_DATA_mask);
#endif
}

const struct uart_ops niiet_uart_ops = {
		.uart_setup = niiet_uart_setup,
		.uart_putc  = niiet_uart_putc,
		.uart_hasrx = niiet_uart_hasrx,
		.uart_getc  = niiet_uart_getc,
		.uart_irq_en = niiet_uart_irq_enable,
		.uart_irq_dis = niiet_uart_irq_disable,
};


