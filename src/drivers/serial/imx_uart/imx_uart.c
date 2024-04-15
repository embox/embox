/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/iomuxc.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/field.h>

#include "imx_uart_regs.h"

#define PIN_CONFIG OPTION_GET(BOOLEAN, pin_config)
#define UART_CLK   OPTION_GET(NUMBER, uart_clk)

static void imx_uart_configure_pins(void) {
	/* We need to configure UART pins to use them as
	 * RXD/TXD. To do it, we need first to config MUX mode register
	 * and then set source for the UART like follows; look manual for
	 * more details */
#if PIN_CONFIG
	switch (UART_NUM) {
	case 0:
		/* TX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7, 1);
		iomuxc_write(IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT, 2);
		/* RX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6, 1);
		iomuxc_write(IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT, 3);
		break;
	case 1:
		/* Nothing to be done */
		break;
	case 2:
		/* TX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA24, 2);
		iomuxc_write(IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT, 0);
		/* RX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25, 2);
		iomuxc_write(IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT, 1);
		break;
	case 3:
		/* TX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_COL0, 4);
		iomuxc_write(IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT, 0);
		/* RX */
		iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0, 4);
		iomuxc_write(IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT, 1);
		break;
	}
#endif
}

#define IMX_UCR2_VALUE \
	IMX_UCR2_SRST | IMX_UCR2_RXEN | IMX_UCR2_TXEN | IMX_UCR2_WS | IMX_UCR2_IRTS
#define IMX_UCR3_VALUE \
	IMX_UCR3_RXDMUXSEL | IMX_UCR3_RI | IMX_UCR3_DCD | IMX_UCR3_DSR

static int imx_uart_setup(struct uart *dev, const struct uart_params *params) {
	uint32_t uart_reg;
	int denom;

	denom = UART_CLK / params->baud_rate;

	imx_uart_configure_pins();

	/* Reset */
	REG32_STORE(IMX_UCR1(dev->base_addr), 0);
	REG32_STORE(IMX_UCR2(dev->base_addr), 0);
	while (REG32_LOAD(IMX_UTS(dev->base_addr)) & IMX_UTS_SOFTRST) {}

	REG32_STORE(IMX_UCR2(dev->base_addr), IMX_UCR2_VALUE);
	REG32_STORE(IMX_UCR3(dev->base_addr), IMX_UCR3_VALUE);
	REG32_STORE(IMX_UCR4(dev->base_addr), 0);

	uart_reg = REG32_LOAD(IMX_UFCR(dev->base_addr));
	uart_reg = FIELD_SET(uart_reg, IMX_UFCR_RFDIV, IMX_UFCR_RFDIV_2);
	uart_reg = FIELD_SET(uart_reg, IMX_UFCR_RXTL, 0x1);
	REG32_STORE(IMX_UFCR(dev->base_addr), uart_reg);

	uart_reg = REG32_LOAD(IMX_UBIR(dev->base_addr));
	uart_reg = FIELD_SET(uart_reg, IMX_UBIR_INC, 0xf);
	REG32_STORE(IMX_UBIR(dev->base_addr), uart_reg);

	uart_reg = REG32_LOAD(IMX_UBMR(dev->base_addr));
	uart_reg = FIELD_SET(uart_reg, IMX_UBMR_MOD, denom);
	REG32_STORE(IMX_UBMR(dev->base_addr), uart_reg);

	REG32_STORE(IMX_UMCR(dev->base_addr), 0);

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(IMX_UCR1(dev->base_addr),
		    IMX_UCR1_UARTEN | IMX_UCR1_RRDYEN);
	}
	else {
		REG32_STORE(IMX_UCR1(dev->base_addr), IMX_UCR1_UARTEN);
	}

	return 0;
}

static int imx_uart_has_symbol(struct uart *dev) {
	return REG32_LOAD(IMX_USR2(dev->base_addr)) & IMX_USR2_RDR;
}

static int imx_uart_getc(struct uart *dev) {
	return FIELD_GET(REG32_LOAD(IMX_URXD(dev->base_addr)), IMX_URXD_DATA);
}

static int imx_uart_putc(struct uart *dev, int ch) {
	uint32_t uart_reg;

	uart_reg = REG32_LOAD(IMX_UTXD(dev->base_addr));
	uart_reg = FIELD_SET(uart_reg, IMX_UTXD_DATA, ch);
	REG32_STORE(IMX_UTXD(dev->base_addr), uart_reg);

	while (!(REG32_LOAD(IMX_UTS(dev->base_addr)) & IMX_UTS_TXEMPTY)) {}

	return 0;
}

static int imx_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	REG32_ORIN(IMX_UCR1(dev->base_addr), IMX_UCR1_RRDYEN);

	return 0;
}

static int imx_uart_irq_dis(struct uart *dev,
    const struct uart_params *params) {
	REG32_CLEAR(IMX_UCR1(dev->base_addr), IMX_UCR1_RRDYEN);

	return 0;
}

const struct uart_ops imx_uart_ops = {
    .uart_getc = imx_uart_getc,
    .uart_putc = imx_uart_putc,
    .uart_hasrx = imx_uart_has_symbol,
    .uart_setup = imx_uart_setup,
    .uart_irq_en = imx_uart_irq_en,
    .uart_irq_dis = imx_uart_irq_dis,
};
