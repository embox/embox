/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/i2c/i2c.h>
#include <drivers/iomuxc.h>

#include <embox/unit.h>

#include "imx_i2c.h"

EMBOX_UNIT_INIT(imx_i2c2_init);

#define I2C2_PIN_SEL  OPTION_GET(NUMBER,i2c2_pins_select)

#define IMX_I2C_BASE     OPTION_GET(NUMBER,base_addr)

#define IMX_I2C_IRQ_NUM  OPTION_GET(NUMBER,irq_num)

static struct imx_i2c imx_i2c2_priv = {
	.irq_num = IMX_I2C_IRQ_NUM,
	.base_addr = IMX_I2C_BASE,
};

static struct i2c_adapter imx_i2c2_adap = {
	.i2c_algo_data = &imx_i2c2_priv,
	.i2c_algo = &imx_i2c_algo,
};

static inline void imx_i2c2_pins_init(void) {
#if I2C2_PIN_SEL
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_COL3, 0x14);
	iomuxc_write(IOMUXC_I2C2_SCL_IN_SELECT_INPUT, 1);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3, 0x14);
	iomuxc_write(IOMUXC_I2C2_SDA_IN_SELECT_INPUT, 1);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B, 6);
	iomuxc_write(IOMUXC_I2C2_SCL_IN_SELECT_INPUT, 0);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16, 6);
	iomuxc_write(IOMUXC_I2C2_SDA_IN_SELECT_INPUT, 0);
#endif
}

static int imx_i2c2_init(void) {
	imx_i2c2_pins_init();
	clk_enable("i2c2");

	return i2c_bus_register(&imx_i2c2_adap, 2, "i2c2");
}

PERIPH_MEMORY_DEFINE(imx_i2c2, IMX_I2C_BASE, 0x100);
