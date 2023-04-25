/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#include <stdint.h>
#include <framework/mod/options.h>
#include <drivers/i2c/i2c.h>

#include "lsm6dsl.h"
#include "lsm6dsl_transfer.h"

#define LSM6DSL_I2C_ADDR  0x6a
#define LSM6DSL_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct lsm6dsl_dev {
	int i2c_bus;
	int i2c_addr;
};

struct lsm6dsl_dev lsm6dsl_dev0 = {
	.i2c_bus  = LSM6DSL_I2C_BUS,
	.i2c_addr = LSM6DSL_I2C_ADDR
};

int lsm6dsl_hw_init(struct lsm6dsl_dev *dev) {
	/* Do nothing */
	return 0;
}

int lsm6dsl_readb(struct lsm6dsl_dev *dev, int offset, uint8_t *ret) {
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	        (uint8_t *) &offset, 1) < 0) {
		return -1;
	}
	if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
		return -1;
	}
	return 0;
}

int lsm6dsl_writeb(struct lsm6dsl_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
