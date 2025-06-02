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
#define IMX_I2C_ID   2

static void imx_i2c_pins2_init(void) {
#if I2C_PIN_SEL
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_COL3, 0x14);
	iomuxc_write(IOMUXC_I2C2_SCL_IN_SELECT_INPUT, 1);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3, 0x14);
	iomuxc_write(IOMUXC_I2C2_SDA_IN_SELECT_INPUT, 1);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B, 0x16);
	iomuxc_write(IOMUXC_I2C2_SCL_IN_SELECT_INPUT, 0);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16, 0x16);
	iomuxc_write(IOMUXC_I2C2_SDA_IN_SELECT_INPUT, 0);
#endif

	clk_enable("i2c2");
}

static const struct imx_i2c_priv imx_i2c2_priv = {
    .init_pins = imx_i2c_pins2_init,
    .base_addr = IMX_I2C_BASE,
    .irq_num = IMX_I2C_IRQ,
};

static const struct i2c_bus imx_i2c2_bus = {
    .i2c_priv = (void *)&imx_i2c2_priv,
    .i2c_ops = &imx_i2c_ops,
    .i2c_id = IMX_I2C_ID,
};

I2C_BUS_REGISTER(&imx_i2c2_bus);

PERIPH_MEMORY_DEFINE(imx_i2c2, IMX_I2C_BASE, 0x100);
