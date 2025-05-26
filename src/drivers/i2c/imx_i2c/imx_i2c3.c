/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/clk/ccm_imx6.h>
#include <drivers/common/memory.h>
#include <drivers/i2c/i2c.h>
#include <drivers/iomuxc.h>
#include <hal/reg.h>
#include <util/log.h>

#include "imx_i2c.h"

#define I2C_PIN_SEL  OPTION_GET(NUMBER, i2c_pins_select)
#define IMX_I2C_BASE OPTION_GET(NUMBER, base_addr)
#define IMX_I2C_IRQ  OPTION_GET(NUMBER, irq_num)
#define IMX_I2C_ID   3

static void imx_i2c3_pins_init(void) {
#if I2C_PIN_SEL == 1
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO03, 0x12);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 0x11);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO06, 0x12);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 0x11);
#elif I2C_PIN_SEL == 2
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO05, 0x16);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 0x12);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO16, 0x16);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 0x12);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17, 0x16);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 0x10);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18, 0x16);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 0x10);
#endif

	clk_enable("i2c3");
}

static const struct imx_i2c_priv imx_i2c3_priv = {
    .init_pins = imx_i2c3_pins_init,
    .base_addr = IMX_I2C_BASE,
    .irq_num = IMX_I2C_IRQ,
};

static const struct i2c_bus imx_i2c3_bus = {
    .i2c_priv = (void *)&imx_i2c3_priv,
    .i2c_ops = &imx_i2c_ops,
    .i2c_id = IMX_I2C_ID,
};

I2C_BUS_REGISTER(&imx_i2c3_bus);

PERIPH_MEMORY_DEFINE(imx_i2c3, IMX_I2C_BASE, 0x100);
