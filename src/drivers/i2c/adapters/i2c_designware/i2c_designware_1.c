/**
 * @file
 *
 * @date Sep 25, 2019
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/i2c/i2c.h>

#include "i2c_designware.h"

#include <embox/unit.h>

EMBOX_UNIT_INIT(i2c_dw_1_init);

#define I2C_DW_BASE     OPTION_GET(NUMBER,base_addr)
#define I2C_DW_IRQ_NUM  OPTION_GET(NUMBER,irq_num)

static struct i2c_dw_dev i2c_dw_1_priv = {
	.irq_num = I2C_DW_IRQ_NUM,
	.base_addr = I2C_DW_BASE,
};

static struct i2c_adapter i2c_dw_1_adap = {
	.i2c_algo_data = &i2c_dw_1_priv,
	.i2c_algo = &i2c_dw_algo,
};

static int i2c_dw_1_init(void) {
	i2c_dw_master_init(&i2c_dw_1_priv);
	return i2c_bus_register(&i2c_dw_1_adap, 1, "i2c1");
}

PERIPH_MEMORY_DEFINE(i2c_dw_1_mem, I2C_DW_BASE, 0x100);
