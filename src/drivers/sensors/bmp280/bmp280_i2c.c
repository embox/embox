/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */
#include <stdint.h>
#include <framework/mod/options.h>
#include <drivers/i2c/i2c.h>

#include "bmp280.h"
#include "bmp280_transfer.h"
#include "drivers/i2c/i2c_bus.h"

#define BMP280_I2C_ADDR  0x76
#define BMP280_I2C_BUS   OPTION_GET(NUMBER, i2c_bus)

struct bmp280_dev {
	int i2c_bus;
	int i2c_addr;
};

struct bmp280_dev bmp280_dev0 = {
	.i2c_bus  = BMP280_I2C_BUS,
	.i2c_addr = BMP280_I2C_ADDR
};

int bmp280_hw_init(struct bmp280_dev *dev) {

	return 0;
}

int bmp280_readb(struct bmp280_dev *dev, int offset, uint8_t *ret) {

	struct i2c_msg msgs[2] = {};
    unsigned nmsgs = 2;

   
    msgs[0].addr = dev->i2c_addr;
    msgs[0].flags = 0;
    msgs[0].buf = (uint8_t *)(&offset);
    msgs[0].len = 1;
    
    msgs[1].addr = dev->i2c_addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = ret;
    msgs[1].len = 1;

	i2c_bus_transfer(dev->i2c_bus, msgs, nmsgs);


	// if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr,
	//         (uint8_t *) &offset, 1) < 0) {
	// 	return -1;
	// }
	// if (i2c_bus_read(dev->i2c_bus, dev->i2c_addr, ret, 1) < 0) {
	// 	return -1;
	// }
	return 0;
}

int bmp280_writeb(struct bmp280_dev *dev,
	    int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	if (i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, 2)) {
		return -1;
	}
	return 0;
}
