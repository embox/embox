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

EMBOX_UNIT_INIT(imx_i2c3_init);

#define I2C3_PIN_SEL  OPTION_GET(NUMBER,i2c3_pins_select)

#define IMX_I2C_BASE     OPTION_GET(NUMBER,base_addr)

#define IMX_I2C_IRQ_NUM  OPTION_GET(NUMBER,irq_num)

static struct imx_i2c imx_i2c3_priv = {
	.irq_num = IMX_I2C_IRQ_NUM,
	.base_addr = IMX_I2C_BASE,
};

static struct i2c_adapter imx_i2c3_adap = {
	.i2c_algo_data = &imx_i2c3_priv,
	.i2c_algo = &imx_i2c_algo,
};

static inline void imx_i2c3_pins_init(void) {
#if I2C3_PIN_SEL == 1
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO03, 2);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 1);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO06, 2);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 1);
#elif I2C3_PIN_SEL == 2
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO05, 6);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 2);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_GPIO16, 6);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 2);
#else
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17, 6);
	iomuxc_write(IOMUXC_I2C3_SCL_IN_SELECT_INPUT, 0);

	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18, 6);
	iomuxc_write(IOMUXC_I2C3_SDA_IN_SELECT_INPUT, 0);
#endif
}

static int imx_i2c3_init(void) {
	imx_i2c3_pins_init();
	clk_enable("i2c3");

	return i2c_bus_register(&imx_i2c3_adap, 3, "i2c3");
}

PERIPH_MEMORY_DEFINE(imx_i2c3, IMX_I2C_BASE, 0x100);
