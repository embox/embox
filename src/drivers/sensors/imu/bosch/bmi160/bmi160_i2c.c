/**
 * @file
 * @brief
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */
#include <stdint.h>
#include <framework/mod/options.h>
#include <drivers/i2c/i2c.h>

#include "bmi160.h"
#include "bmi160_transfer.h"

/* Connecting SA0 to GND sets the I2C address to 0x68. 
If connected to 3.3V, the I2C address would change to 0x69. */
#define BMI160_I2C_ADDR  OPTION_GET(NUMBER, i2c_addr)
#define BMI160_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct bmi160_dev {
	int i2c_bus;
	int i2c_addr;
};

struct bmi160_dev bmi160_dev0 = {
	.i2c_bus  = BMI160_I2C_BUS,
	.i2c_addr = BMI160_I2C_ADDR
};

int bmi160_hw_init(struct bmi160_dev *dev) {
	/* Do nothing */
	return 0;
}

int bmi160_readb(struct bmi160_dev *dev, int offset, uint8_t *ret) {
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	        (uint8_t *) &offset, 1) < 0) {
		return -1;
	}
	if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
		return -1;
	}
	return 0;
}

int bmi160_writeb(struct bmi160_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
