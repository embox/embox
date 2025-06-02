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
#define IMX_I2C_ID   1

static void imx_i2c1_pins_init(void) {
#if I2C_PIN_SEL
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09, 0x14);
	iomuxc_write(IOMUXC_I2C1_SCL_IN_SELECT_INPUT, 1);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08, 0x14);
	iomuxc_write(IOMUXC_I2C1_SDA_IN_SELECT_INPUT, 1);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA21, 0x16);
	iomuxc_write(IOMUXC_I2C1_SCL_IN_SELECT_INPUT, 0);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA28, 0x11);
	iomuxc_write(IOMUXC_I2C1_SDA_IN_SELECT_INPUT, 0);
#endif

	clk_enable("i2c1");
}

static const struct imx_i2c_priv imx_i2c1_priv = {
    .init_pins = imx_i2c1_pins_init,
    .base_addr = IMX_I2C_BASE,
    .irq_num = IMX_I2C_IRQ,
};

static const struct i2c_bus imx_i2c1_bus = {
    .i2c_priv = (void *)&imx_i2c1_priv,
    .i2c_ops = &imx_i2c_ops,
    .i2c_id = IMX_I2C_ID,
};

I2C_BUS_REGISTER(&imx_i2c1_bus);

PERIPH_MEMORY_DEFINE(imx_i2c1, IMX_I2C_BASE, 0x100);
