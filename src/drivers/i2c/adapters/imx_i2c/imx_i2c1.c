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

EMBOX_UNIT_INIT(imx_i2c1_init);

#define I2C1_PIN_SEL  OPTION_GET(NUMBER,i2c1_pins_select)

#define IMX_I2C_BASE     OPTION_GET(NUMBER,base_addr)

#define IMX_I2C_IRQ_NUM  OPTION_GET(NUMBER,irq_num)

static struct imx_i2c imx_i2c1_priv = {
	.irq_num = IMX_I2C_IRQ_NUM,
	.base_addr = IMX_I2C_BASE,
};

static struct i2c_adapter imx_i2c1_adap = {
	.i2c_algo_data = &imx_i2c1_priv,
	.i2c_algo = &imx_i2c_algo,
};

static inline void imx_i2c1_pins_init(void) {
#if I2C1_PIN_SEL
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09, 4);
	iomuxc_write(IOMUXC_I2C1_SCL_IN_SELECT_INPUT, 1);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08, 4);
	iomuxc_write(IOMUXC_I2C1_SDA_IN_SELECT_INPUT, 1);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA21, 6);
	iomuxc_write(IOMUXC_I2C1_SCL_IN_SELECT_INPUT, 0);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA28, 1);
	iomuxc_write(IOMUXC_I2C1_SDA_IN_SELECT_INPUT, 0);
#endif
}

static int imx_i2c1_init(void) {
	imx_i2c1_pins_init();
	clk_enable("i2c1");

	return i2c_bus_register(&imx_i2c1_adap, 1, "i2c1");
}

PERIPH_MEMORY_DEFINE(imx_i2c1, IMX_I2C_BASE, 0x100);
