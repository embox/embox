/**
 * @file
 *
 * @date Dec 2, 2024
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <drivers/i2c/i2c.h>
#include <drivers/char_dev.h>

#include <drivers/eeprom.h>


static ssize_t eeprom_cdev_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct eeprom_dev *dev;
	struct i2c_msg msgs[2] = {0};
	uint8_t msgbuf[2] = {0};
	int i;
	uint32_t offset;
	int count;
	int res;

	dev = (struct eeprom_dev *)cdev;

	memset(msgs, 0, sizeof(msgs));

	count = nbyte;
	if (count > dev->eed_io_limit) {
		count = dev->eed_io_limit;
	}

	offset = dev->eed_offset;

	i = 0;
#if 0
	if (at24->chip.flags & AT24_FLAG_ADDR16) {
		msgbuf[i++] = offset >> 8;
	}
#endif
	msgbuf[i++] = offset;

	msgs[0].addr = dev->eed_bus_addr;
	msgs[0].buf = msgbuf;
	msgs[0].len = i;

	msgs[1].addr = dev->eed_bus_addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = buf;
	msgs[1].len = count;

	res = i2c_bus_transfer(dev->eed_bus, dev->eed_bus_addr, msgs, 2);

	return res;
}

static int eeprom_cdev_status(struct char_dev *cdev, int mask) {
    return 0;
}

static int eeprom_cdev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct eeprom_dev *dev;

	dev = (struct eeprom_dev *)cdev;

	if (dev->eed_idesc) {
		return -EBUSY;
	}

	dev->eed_idesc = idesc;

	eeprom_dev_open(dev);

	return 0;
}

static void eeprom_cdev_close(struct char_dev *cdev) {
	struct eeprom_dev *dev;

	dev = (struct eeprom_dev *)cdev;

	eeprom_dev_close(dev);

	dev->eed_idesc = NULL;
}

const struct char_dev_ops eeprom_cdev_ops = {
    .read = eeprom_cdev_read,
    .status = eeprom_cdev_status,
    .open = eeprom_cdev_open,
    .close = eeprom_cdev_close,
};

int eeprom_dev_private_register(struct eeprom_dev *dev) {
	int err;

	char_dev_init(&dev->eed_cdev, dev->eed_name, &eeprom_cdev_ops);

	err = char_dev_register(&dev->eed_cdev);

	if (err) {
		log_error("failed to register char device for \"%s\"", dev->eed_name);
	}

	return err;
}