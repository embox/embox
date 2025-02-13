/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 11.02.25
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define CLK_FREQ OPTION_GET(NUMBER, clk_freq)
#define SPI_BUS  OPTION_GET(NUMBER, spi_bus)
#define SPI_CS   OPTION_GET(NUMBER, spi_cs)

/**
 * General Register Set
 * 
 * (RO) - read only
 * (WO) - write only
 * (RW) - read/write
 */
#define UART_RHR 0x0 /* (RO) Receiver Holding Reg */
#define UART_THR 0x0 /* (WO) Transmitter Holding Reg */
#define UART_IER 0x1 /* (RW) Interrupt Enable Reg */
#define UART_ISR 0x2 /* (RO) Interrupt Status Reg */
#define UART_FCR 0x2 /* (WO) FIFO Control Reg */
#define UART_LCR 0x3 /* (RW) Line Control Reg */
#define UART_MCR 0x4 /* (RW) Modem Control Reg */
#define UART_LSR 0x5 /* (RO) Line Status Reg */
#define UART_MSR 0x6 /* (RO) Modem Status Reg */
#define UART_SPR 0x7 /* (RW) Scratch Pad Reg */

/**
 * Registers accesible only when DLAB = 1
 */
#define UART_DLL 0x0 /* (RW) Divisor LSB */
#define UART_DLM 0x1 /* (RW) Divisor MSB */
#define UART_PSD 0x5 /* (WO) Prescaler Division */

#define UART_IER_DR   (1U << 0) /* Data Ready */
#define UART_IER_THRE (1U << 1) /* Transmit-hold-register empty */
#define UART_IER_RLS  (1U << 2) /* Receiver Line Status */
#define UART_IER_MS   (1U << 3) /* Modem Status */
#define UART_IER_TE   (1U << 6) /* DMA Rx End */
#define UART_IER_RE   (1U << 7) /* DMA Tx End */

#define UART_LCR_PE   (1U << 3) /* Parity Enable */
#define UART_LCR_EP   (1U << 4) /* Even Parity */
#define UART_LCR_FP   (1U << 5) /* Force Parity */
#define UART_LCR_SB   (1U << 6) /* Set Break */
#define UART_LCR_DLAB (1U << 7) /* Divisor Latch Access Bit */

#define UART_LSR_DR   (1U << 0) /* Data Ready */
#define UART_LSR_OE   (1U << 1) /* Overrun Error */
#define UART_LSR_PE   (1U << 2) /* Parity Error */
#define UART_LSR_FE   (1U << 3) /* Framing Error */
#define UART_LSR_BI   (1U << 4) /* Break Interrupt */
#define UART_LSR_THRE (1U << 5) /* Transmit-hold-register empty */
#define UART_LSR_TE   (1U << 6) /* Transmitter empty */
#define UART_LSR_FDE  (1U << 7) /* FIFO data Error */

static struct spi_device *spi_bus_dev;

#define UART_REG_LOAD(reg)                      \
	({                                          \
		uint8_t buf[1];                         \
		buf[0] = (reg << 3) | (1 << 7);         \
		spi_transfer(spi_bus_dev, buf, buf, 1); \
		buf[0];                                 \
	})

#define UART_REG_STORE(reg, val)                 \
	({                                           \
		uint8_t buf[2];                          \
		buf[0] = reg << 3;                       \
		buf[1] = val;                            \
		spi_transfer(spi_bus_dev, buf, NULL, 2); \
	})

#define UART_REG_ORIN(reg, val) \
	UART_REG_STORE(reg, UART_REG_LOAD(reg) | ((uint8_t)val))

#define UART_REG_ANDIN(reg, val) \
	UART_REG_STORE(reg, UART_REG_LOAD(reg) & ((uint8_t)val))

#define UART_REG_CLEAR(reg, val) \
	UART_REG_STORE(reg, UART_REG_LOAD(reg) & ~((uint8_t)val))

static int xr20m1172_setup(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		UART_REG_ORIN(UART_IER, UART_IER_DR);
	}

#if CLK_FREQ
	uint16_t baud_divisor;
	uint8_t lcr;

	baud_divisor = (CLK_FREQ + (params->baud_rate * 8))
	               / (params->baud_rate * 16);

	lcr = UART_REG_LOAD(UART_LCR);
	UART_REG_STORE(UART_LCR, lcr | UART_LCR_DLAB);
	UART_REG_STORE(UART_DLL, baud_divisor & 0xff);
	UART_REG_STORE(UART_DLM, (baud_divisor >> 8) & 0xff);
	UART_REG_STORE(UART_LCR, lcr);
#endif

	return 0;
}

static int xr20m1172_irq_en(struct uart *dev,
    const struct uart_params *params) {
	UART_REG_ORIN(UART_IER, UART_IER_DR);

	return 0;
}

static int xr20m1172_irq_dis(struct uart *dev,
    const struct uart_params *params) {
	UART_REG_CLEAR(UART_IER, UART_IER_DR);

	return 0;
}

static int xr20m1172_putc(struct uart *dev, int ch) {
	/* FIXME */
	// while (!(UART_REG_LOAD(UART_LSR) & UART_LSR_THRE)) {}

	UART_REG_STORE(UART_THR, ch);

	return 0;
}

static int xr20m1172_getc(struct uart *dev) {
	return UART_REG_LOAD(UART_RHR);
}

static int xr20m1172_has_symbol(struct uart *dev) {
	return UART_REG_LOAD(UART_LSR) & UART_LSR_DR;
}

const struct uart_ops xr20m1172_uart_ops = {
    .uart_getc = xr20m1172_getc,
    .uart_putc = xr20m1172_putc,
    .uart_hasrx = xr20m1172_has_symbol,
    .uart_setup = xr20m1172_setup,
    .uart_irq_en = xr20m1172_irq_en,
    .uart_irq_dis = xr20m1172_irq_dis,
};

EMBOX_UNIT_INIT(xr20m1172_init);

static int xr20m1172_init(void) {
	spi_bus_dev = spi_dev_by_id(SPI_BUS);
	spi_bus_dev->flags |= SPI_CS_ACTIVE;
	spi_bus_dev->flags |= SPI_CS_INACTIVE;
	spi_select(spi_bus_dev, SPI_CS);
	return 0;
}
