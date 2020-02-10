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

#include "l3g4200d.h"
#include "l3g4200d_transfer.h"

#define L3G4200D_I2C_ADDR  0x69
#define L3G4200D_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct l3g4200d_dev {
	int i2c_bus;
	int i2c_addr;
};

struct l3g4200d_dev l3g4200d_dev0 = {
	.i2c_bus  = L3G4200D_I2C_BUS,
	.i2c_addr = L3G4200D_I2C_ADDR
};

int l3g4200d_hw_init(struct l3g4200d_dev *dev) {
	/* Do nothing */
	return 0;
}

int l3g4200d_readb(struct l3g4200d_dev *dev, int offset, uint8_t *ret) {
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	        (uint8_t *) &offset, 1) < 0) {
		return -1;
	}
	if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
		return -1;
	}
	return 0;
}

int l3g4200d_writeb(struct l3g4200d_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
