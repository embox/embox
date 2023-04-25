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

#include "lis3mdl.h"
#include "lis3mdl_transfer.h"

#define LIS3MDL_I2C_ADDR  0x1e
#define LIS3MDL_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct lis3mdl_dev {
	int i2c_bus;
	int i2c_addr;
};

struct lis3mdl_dev lis3mdl_dev0 = {
	.i2c_bus  = LIS3MDL_I2C_BUS,
	.i2c_addr = LIS3MDL_I2C_ADDR
};

int lis3mdl_hw_init(struct lis3mdl_dev *dev) {
	/* Do nothing */
	return 0;
}

int lis3mdl_readb(struct lis3mdl_dev *dev, int offset, uint8_t *ret) {
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	        (uint8_t *) &offset, 1) < 0) {
		return -1;
	}
	if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
		return -1;
	}
	return 0;
}

int lis3mdl_writeb(struct lis3mdl_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
