/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */

#include <stddef.h>
#include <stdint.h>

#include <drivers/i2c/i2c.h>
#include <framework/mod/options.h>

#include "bmp280.h"
#include "bmp280_transfer.h"
#include "drivers/i2c/i2c_bus.h"

#define BMP280_I2C_ADDR 0x76
#define BMP280_I2C_BUS  OPTION_GET(NUMBER, i2c_bus)

struct bmp280_dev {
	int i2c_bus;
	int i2c_addr;
};

struct bmp280_dev bmp280_dev0 = {.i2c_bus = BMP280_I2C_BUS,
    .i2c_addr = BMP280_I2C_ADDR};

int bmp280_hw_init(struct bmp280_dev *dev) {
	return 0;
}

int bmp280_read(struct bmp280_dev *dev, int offset, uint8_t *ret, size_t len) {
	uint8_t reg = offset;
	struct i2c_msg msgs[] = {
	    {
	        .addr = dev->i2c_addr,
	        .buf = &reg,
	        .len = 1,
	    },
	    {
	        .addr = dev->i2c_addr,
	        .flags = I2C_M_RD,
	        .buf = ret,
	        .len = len,
	    },
	};
	int res;

	res = i2c_bus_transfer(dev->i2c_bus, msgs, 2);
	return res < 0 ? res : 0;
}

int bmp280_readb(struct bmp280_dev *dev, int offset, uint8_t *ret) {
	return bmp280_read(dev, offset, ret, 1);
}

int bmp280_writeb(struct bmp280_dev *dev, int offset, uint8_t val) {
	uint8_t data[] = {offset, val};
	int res;

	res = i2c_bus_write(dev->i2c_bus, dev->i2c_addr, data, sizeof(data));
	return res < 0 ? res : 0;
}
