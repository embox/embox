/**
 * @file adxl345.c
 * @brief G-Sensor
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 17.12.2019
 */

#include <stdint.h>

#include <drivers/i2c/i2c.h>
#include <drivers/sensors/adxl345.h>
#include <framework/mod/options.h>
#include <util/log.h>

#define ADXL345_DEVID           0x00
#define ADXL345_OFSX            0x1E
#define ADXL345_OFSY            0x1F
#define ADXL345_OFSZ            0x20
#define ADXL345_OFS_AXIS(index) (ADXL345_REG_OFSX + (index))
#define ADXL345_BW_RATE         0x2C
#define ADXL345_POWER_CTL       0x2D
# define ADXL345_POWER_CTL_MSR  (1 << 3)
#define ADXL345_DATA_FORMAT     0x31
#define ADXL345_DATAX0          0x32
#define ADXL345_DATAY0          0x34
#define ADXL345_DATAZ0          0x36
#define ADXL345_DATA_AXIS(n)    (ADXL345_DATAX0 + (n))

#define ADXL345_DEVID_VALUE     0xE5

#define ADXL345_BUS    OPTION_GET(NUMBER, bus)
#define ADXL345_ADDR   OPTION_GET(NUMBER, addr)

struct adxl345_dev {
	int i2c_bus;
	int i2c_addr;
};

/* Note: ADXL345 supports i2c and spi read/write ops,
 * but for now we use only i2c bus */
static uint8_t adxl345_readb(struct adxl345_dev *dev, int offset) {
	uint8_t ret = offset;
	i2c_bus_write(dev->i2c_bus, dev->i2c_addr, &ret, sizeof(ret));
	i2c_bus_read(dev->i2c_bus, dev->i2c_addr, &ret, sizeof(ret));
	return ret;
}

static void adxl345_writeb(struct adxl345_dev *dev, int offset, uint8_t val) {
	uint16_t tmp = (offset & 0xFF) | (val << 8);
	i2c_bus_write(dev->i2c_bus, dev->i2c_addr, (void *) &tmp, sizeof(tmp));
}

static struct adxl345_dev adxl345_dev = {
	.i2c_bus  = ADXL345_BUS,
	.i2c_addr = ADXL345_ADDR
};

int16_t adxl345_get_x(void) {
	struct adxl345_dev *dev = &adxl345_dev;
	return (int16_t) (adxl345_readb(dev, ADXL345_DATA_AXIS(0)) +
		(adxl345_readb(dev, ADXL345_DATA_AXIS(1)) << 8));
}

int16_t adxl345_get_y(void) {
	struct adxl345_dev *dev = &adxl345_dev;
	return (int16_t) (adxl345_readb(dev, ADXL345_DATA_AXIS(2)) +
		(adxl345_readb(dev, ADXL345_DATA_AXIS(3)) << 8));
}

int16_t adxl345_get_z(void) {
	struct adxl345_dev *dev = &adxl345_dev;
	return (int16_t) (adxl345_readb(dev, ADXL345_DATA_AXIS(4)) +
		(adxl345_readb(dev, ADXL345_DATA_AXIS(5)) << 8));
}

int adxl345_init(void) {
	uint8_t tmp;
	struct adxl345_dev *dev = &adxl345_dev;

	tmp = adxl345_readb(dev, ADXL345_DEVID);
	if (tmp != ADXL345_DEVID_VALUE) {
		log_error("ADXL345 Device ID mismatch! %2x", tmp);
		return -1;
	}

	adxl345_writeb(dev, ADXL345_POWER_CTL,
			ADXL345_POWER_CTL_MSR);

	return 0;
}
