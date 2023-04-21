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

#include "lps22hb.h"
#include "lps22hb_transfer.h"

#define LPS22HB_I2C_ADDR  0x5d
#define LPS22HB_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct lps22hb_dev {
	int i2c_bus;
	int i2c_addr;
};

struct lps22hb_dev lps22hb_dev0 = {
	.i2c_bus  = LPS22HB_I2C_BUS,
	.i2c_addr = LPS22HB_I2C_ADDR
};

int lps22hb_hw_init(struct lps22hb_dev *dev) {
	/* Do nothing */
	return 0;
}

int lps22hb_readb(struct lps22hb_dev *dev, int offset, uint8_t *ret) {
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	        (uint8_t *) &offset, 1) < 0) {
		return -1;
	}
	if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
		return -1;
	}
	return 0;
}

int lps22hb_writeb(struct lps22hb_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
