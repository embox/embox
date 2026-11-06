/**
 * @file
 *
 * @date Decab, 2025
 * @author  Efim Perevalov
 */
// #include <assert.h>
#include <errno.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/esp32_intmatrix.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#if defined(__EMBOX_PLATFORM__esp32c5__)
#include <config/board_config.h>
#include <drivers/clk/esp32c5_pcr.h>

#define UART0_BASE_ADDR   0x60000000U
#define UART1_BASE_ADDR   0x60001000U
#define UART0_INTR_SOURCE 47U
#define UART1_INTR_SOURCE 48U
#endif

#if defined(__EMBOX_PLATFORM__esp32c5__)
#define UART_ERR_WR_MASK_M (1U << 18)
#define UART_MEM_CLK_EN_S 20
#else
#define UART_ERR_WR_MASK_M (1U << 26)
#define UART_MEM_CLK_EN_S 24
#endif

#define UART_MEM_CLK_EN_V 0x1
#define UART_MEM_CLK_EN_M ((UART_MEM_CLK_EN_V) << (UART_MEM_CLK_EN_S))

#define UART_TXFIFO_EMPTY_THRHD_V 0x7F
#define UART_TXFIFO_EMPTY_THRHD_S 8
#define UART_TXFIFO_EMPTY_THRHD_M \
	((UART_TXFIFO_EMPTY_THRHD_V) << (UART_TXFIFO_EMPTY_THRHD_S))

#define UART_RXFIFO_FULL_THRHD_V 0x7F
#define UART_RXFIFO_FULL_THRHD_S 0
#define UART_RXFIFO_FULL_THRHD_M \
	((UART_RXFIFO_FULL_THRHD_V) << (UART_RXFIFO_FULL_THRHD_S))

#define UART_TX_SIZE_V 0xF
#define UART_TX_SIZE_S 7
#define UART_TX_SIZE_M ((UART_TX_SIZE_V) << (UART_TX_SIZE_S))

#define UART_RX_SIZE_V 0xF
#define UART_RX_SIZE_S 3
#define UART_RX_SIZE_M ((UART_RX_SIZE_V) << (UART_RX_SIZE_S))

/* FIFO Configuration */
#define UART_FIFO(base)     (base + 0x0000) /* RO */
#define UART_MEM_CONF(base) (base + 0x0060) /* R/W */

/* UART Interrupt Register */
#define UART_INT_RAW(base) (base + 0x0004) /* R/WTC/SS */
#define UART_INT_ST(base)  (base + 0x0008) /* RO */
#define UART_INT_ENA(base) (base + 0x000C) /* R/W */
#define UART_INT_CLR(base) (base + 0x0010) /* WT */

#define UART_RXFIFO_FULL_INT (1U << 0)

/* Configuration Register */
#define UART_CLKDIV(base)     (base + 0x0014) /* R/W */
#define UART_RX_FILT(base)    (base + 0x0018) /* R/W */
#define UART_CONF0(base)      (base + 0x0020) /* R/W */
#define UART_CONF1(base)      (base + 0x0024) /* R/W */
#define UART_FLOW_CONF(base)  (base + 0x0034) /* varies */
#define UART_SLEEP_CONF(base) (base + 0x0038) /* R/W */
#define UART_SWFC_CONF0(base) (base + 0x003C) /* R/W */
#define UART_SWFC_CONF1(base) (base + 0x0040) /* R/W */
#define UART_TXBRK_CONF(base) (base + 0x0044) /* R/W */
#define UART_IDLE_CONF(base)  (base + 0x0048) /* R/W */
#define UART_RS485_CONF(base) (base + 0x004C) /* R/W */
#define UART_CLK_CONF(base)   (base + 0x0078) /* R/W */

/* Status Register */
#define UART_STATUS(base)        (base + 0x001C) /* R/O */
#define UART_MEM_TX_STATUS(base) (base + 0x0064) /* R/O */
#define UART_MEM_RX_STATUS(base) (base + 0x0068) /* R/O */
#if defined(__EMBOX_PLATFORM__esp32c5__)
#define UART_FSM_STATUS(base) (base + 0x0070) /* R/O */
#else
#define UART_FSM_STATUS(base) (base + 0x006C) /* R/O */
#endif

/* Autobaud Register */
#define UART_LOWPULSE(base)  (base + 0x0028) /* R/O */
#define UART_HIGHPULSE(base) (base + 0x002C) /* R/O */
#define UART_RXD_CNT(base)   (base + 0x0030) /* R/O */
#define UART_POSPULSE(base)  (base + 0x0070) /* R/O */
#define UART_NEGPULSE(base)  (base + 0x0074) /* R/O */

/* AT Escape Sequence Selection Configuration */
#define UART_AT_CMD_PRECNT(base)  (base + 0x0050)   /* R/W */
#define UART_AT_CMD_POSTCNT(base) (base + 0x0054)   /* R/W */
#define UART_AT_CMD_GAPTOUT(base) ((base) + 0x0058) /* R/W */
#define UART_AT_CMD_CHAR(base)    ((base) + 0x005C) /* R/W */

/* Version Register */
#define UART_DATE(base) ((base) + 0x007C) /* R/W */
#define UART_ID(base)   ((base) + 0x0080) /* varies */

/* Configuration Register */
#define UHCI_CONF0(base)        ((base) + 0x0000) /* R/W */
#define UHCI_CONF1(base)        ((base) + 0x0014) /* varies*/
#define UHCI_ESCAPE_CONF(base)  ((base) + 0x0020) /* R/W */
#define UHCI_HUNG_CONF(base)    ((base) + 0x0024) /* R/W */
#define UHCI_ACK_NUM(base)      ((base) + 0x0028) /* varies */
#define UHCI_QUICK_SENT(base)   ((base) + 0x0030) /* varies */
#define UHCI_REG_Q0_WORD0(base) ((base) + 0x0034) /* R/W */
#define UHCI_REG_Q0_WORD1(base) ((base) + 0x0038) /* R/W */
#define UHCI_REG_Q1_WORD0(base) ((base) + 0x003C) /* R/W */
#define UHCI_REG_Q1_WORD1(base) ((base) + 0x0040) /* R/W */
#define UHCI_REG_Q2_WORD0(base) ((base) + 0x0044) /* R/W */
#define UHCI_REG_Q2_WORD1(base) ((base) + 0x0048) /* R/W */
#define UHCI_REG_Q3_WORD0(base) ((base) + 0x004C) /* R/W */
#define UHCI_REG_Q3_WORD1(base) ((base) + 0x0050) /* R/W */
#define UHCI_REG_Q4_WORD0(base) ((base) + 0x0054) /* R/W */
#define UHCI_REG_Q4_WORD1(base) ((base) + 0x0058) /* R/W */
#define UHCI_REG_Q5_WORD0(base) ((base) + 0x005C) /* R/W */
#define UHCI_REG_Q5_WORD1(base) ((base) + 0x0060) /* R/W */
#define UHCI_REG_Q6_WORD0(base) ((base) + 0x0064) /* R/W */
#define UHCI_REG_Q6_WORD1(base) ((base) + 0x0068) /* R/W */
#define UHCI_ESC_CONF0(base)    ((base) + 0x006C) /* R/W */
#define UHCI_ESC_CONF1(base)    ((base) + 0x0070) /* R/W */
#define UHCI_ESC_CONF2(base)    ((base) + 0x0074) /* R/W */
#define UHCI_ESC_CONF3(base)    ((base) + 0x0078) /* R/W */
#define UHCI_PKT_THRES(base)    ((base) + 0x007C) /* R/W */

/* UHCI Interrupt Register */
#define UHCI_INT_RAW(base) ((base) + 0x0004) /* varies */
#define UHCI_INT_ST(base)  ((base) + 0x0008) /* RO */
#define UHCI_INT_ENA(base) ((base) + 0x000C) /* R/W */
#define UHCI_INT_CLR(base) ((base) + 0x0010) /* WT */

/* HCI Status Register */
#define UHCI_STATE0(base)  ((base) + 0x0018) /* RO */
#define UHCI_STATE1(base)  ((base) + 0x001C) /* RO */
#define UHCI_RX_HEAD(base) ((base) + 0x002c) /* RO */

/* Version Register */
#define UHCI_DATE(base) ((base) + 0x0080) /* R/W */

static int esp32_uart_getc(struct uart *dev) {
	return (int)(REG32_LOAD(UART_FIFO(dev->base_addr)) & 0xFF);
}

static int esp32_uart_putc(struct uart *dev, int ch) {
	while (REG32_LOAD(UART_STATUS(dev->base_addr)) & (0x3FF << 16)) {};

	REG32_STORE(UART_FIFO(dev->base_addr), (uint32_t)(ch & 0xFF));

	return 0;
}

static int esp32_uart_hasrx(struct uart *dev) {
	uint32_t uart_status;
	uint32_t uart_rxfifo_cnt;

	/* Clear before checking FIFO so a newly received byte is not lost. */
	REG32_STORE(UART_INT_CLR(dev->base_addr), UART_RXFIFO_FULL_INT);

	uart_status = REG32_LOAD(UART_STATUS(dev->base_addr));
	uart_rxfifo_cnt = uart_status & 0x3FF;

	if (uart_rxfifo_cnt) {
		return 1;
	};

	return 0;
}

static int esp32_uart_irq_en(struct uart *dev, const struct uart_params *params) {
#if defined(__EMBOX_PLATFORM__esp32c5__)
	uint32_t source;
	int res;

	switch (dev->base_addr) {
	case UART0_BASE_ADDR:
		source = UART0_INTR_SOURCE;
		break;
	case UART1_BASE_ADDR:
		source = UART1_INTR_SOURCE;
		break;
	default:
		return -EINVAL;
	}

	res = esp32_intmatrix_route(source, dev->irq_num);
	if (res != 0) {
		return res;
	}
#endif

	/* Only RX FIFO interrupts are handled; discard ROM interrupt enables. */
	REG32_STORE(UART_INT_ENA(dev->base_addr), UART_RXFIFO_FULL_INT);

	return 0;
}

#if defined(__EMBOX_PLATFORM__esp32c5__)
#define UART_CLKDIV_INT_MASK   0xfffU
#define UART_CLKDIV_FRAG_SHIFT 20
#define UART_CLKDIV_FRAG_MASK  (0xfU << UART_CLKDIV_FRAG_SHIFT)
#define UART_REG_UPDATE(base)  ((base) + 0x98)
#define UART_REG_UPDATE_EN     (1U << 0)
#define UART_POLL_LIMIT        1000000U

static int esp32_uart_set_baudrate(struct uart *dev, uint32_t baud_rate) {
	uint32_t source_div, clk_div, reg, source_hz, rate;
	unsigned int id, source, timeout;
	uint64_t max_rate;
	int ret;

	if (!baud_rate) {
		return -EINVAL;
	}
	switch (dev->base_addr) {
	case UART0_BASE_ADDR:
		id = CONF_UART0_CLK_DEF_ID;
		source = CONF_UART0_CLK_DEF_SOURCE;
		break;
#ifdef CONF_UART1_ENABLED
	case UART1_BASE_ADDR:
		id = CONF_UART1_CLK_DEF_ID;
		source = CONF_UART1_CLK_DEF_SOURCE;
		break;
#endif
	default:
		return -EINVAL;
	}

	ret = esp32_clk_get_source_rate(source, &source_hz);
	if (ret) {
		return ret;
	}
	if (baud_rate > source_hz) {
		return -EINVAL;
	}

	/* Extend the 12-bit UART divider with the PCR integer predivider. */
	max_rate = (uint64_t)UART_CLKDIV_INT_MASK * baud_rate;
	source_div = (source_hz + max_rate - 1) / max_rate;
	if (source_div > 256) {
		return -EINVAL;
	}

	/* Finish any ROM/diag output at the old rate before switching clocks. */
	for (timeout = UART_POLL_LIMIT; timeout; timeout--) {
		if (!(REG32_LOAD(UART_STATUS(dev->base_addr)) & (0xffU << 16))
		    && !(REG32_LOAD(UART_FSM_STATUS(dev->base_addr)) & 0xf0U)) {
			break;
		}
	}
	if (!timeout) {
		return -ETIMEDOUT;
	}

	ret = esp32_clk_configure(id, source, source_div);
	if (ret) {
		return ret;
	}
	ret = esp32_clk_enable(id);
	if (ret) {
		return ret;
	}
	ret = esp32_clk_get_rate(id, &rate);
	if (ret) {
		return ret;
	}
	clk_div = ((uint64_t)rate << 4) / baud_rate;
	reg = REG32_LOAD(UART_CLKDIV(dev->base_addr));
	reg &= ~(UART_CLKDIV_INT_MASK | UART_CLKDIV_FRAG_MASK);
	reg |= clk_div >> 4;
	reg |= (clk_div & 0xfU) << UART_CLKDIV_FRAG_SHIFT;
	REG32_STORE(UART_CLKDIV(dev->base_addr), reg);
	REG32_ORIN(UART_REG_UPDATE(dev->base_addr), UART_REG_UPDATE_EN);
	for (timeout = UART_POLL_LIMIT; timeout; timeout--) {
		if (!(REG32_LOAD(UART_REG_UPDATE(dev->base_addr)) & UART_REG_UPDATE_EN)) {
			return 0;
		}
	}

	return -ETIMEDOUT;
}
#endif

static int esp32_uart_setup(struct uart *dev, const struct uart_params *params) {
#if defined(__EMBOX_PLATFORM__esp32c5__)
	int ret;

	ret = esp32_uart_set_baudrate(dev, params->baud_rate);
	if (ret) {
		return ret;
	}
#endif
	//  modifyreg32(UART_CONF0_REG(priv->id), UART_ERR_WR_MASK_M | UART_MEM_CLK_EN_M, UART_ERR_WR_MASK_M);
	uint32_t conf0 = REG32_LOAD(UART_CONF0(dev->base_addr));
	conf0 &= ~(UART_ERR_WR_MASK_M | UART_MEM_CLK_EN_M);
	conf0 |= UART_ERR_WR_MASK_M;

	REG32_STORE(UART_CONF0(dev->base_addr), conf0);

	// modifyreg32(UART_CONF1_REG(priv->id), UART_TXFIFO_EMPTY_THRHD_M, 0);
	uint32_t conf1 = REG32_LOAD(UART_CONF1(dev->base_addr));
	conf1 &= ~UART_TXFIFO_EMPTY_THRHD_M;
	conf1 |= 0;

	REG32_STORE(UART_CONF1(dev->base_addr), conf1);

	// modifyreg32(UART_CONF1_REG(priv->id), UART_RXFIFO_FULL_THRHD_M, 1 << UART_RXFIFO_FULL_THRHD_S);
	conf1 = REG32_LOAD(UART_CONF1(dev->base_addr));
	conf1 &= ~UART_RXFIFO_FULL_THRHD_M;
	conf1 |= 1 << UART_RXFIFO_FULL_THRHD_S;

	REG32_STORE(UART_CONF1(dev->base_addr), conf1);

#if !defined(__EMBOX_PLATFORM__esp32c5__)
	// modifyreg32(UART_MEM_CONF_REG(priv->id), UART_TX_SIZE_M | UART_RX_SIZE_M, (1 << UART_TX_SIZE_S) | (1 << UART_RX_SIZE_S));
	uint32_t mem_conf_reg = REG32_LOAD(UART_MEM_CONF(dev->base_addr));
	mem_conf_reg &= ~(UART_TX_SIZE_M | UART_RX_SIZE_M);
	mem_conf_reg |= (1 << UART_TX_SIZE_S) | (1 << UART_RX_SIZE_S);
	REG32_STORE(UART_MEM_CONF(dev->base_addr), mem_conf_reg);
#endif

#if defined(__EMBOX_PLATFORM__esp32c5__)
	/* CONF0 is synchronized into the UART clock domain on C5. */
	REG32_ORIN(UART_REG_UPDATE(dev->base_addr), UART_REG_UPDATE_EN);
	unsigned int timeout;
	for (timeout = UART_POLL_LIMIT; timeout; timeout--) {
		if (!(REG32_LOAD(UART_REG_UPDATE(dev->base_addr))
		        & UART_REG_UPDATE_EN)) {
			break;
		}
	}
	if (!timeout) {
		return -ETIMEDOUT;
	}
#endif

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		return esp32_uart_irq_en(dev, params);
	}

	return 0;
}

static int esp32_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	REG32_ANDIN(UART_INT_ENA(dev->base_addr), 0);
	return 0;
}

const struct uart_ops esp32_uart_ops = {
    .uart_getc = esp32_uart_getc,
    .uart_putc = esp32_uart_putc,
    .uart_hasrx = esp32_uart_hasrx,
    .uart_setup = esp32_uart_setup,
    .uart_irq_en = esp32_uart_irq_en,
    .uart_irq_dis = esp32_uart_irq_dis,
};
