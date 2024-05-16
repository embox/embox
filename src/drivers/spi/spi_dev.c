/**
 * @file spi_devfs.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.12.2018
 */

#include <assert.h>
#include <stdint.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/spi.h>
#include <util/log.h>

static ssize_t spi_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct spi_device *dev;

	assert(cdev);
	assert(buf);

	dev = (struct spi_device *)cdev;

	spi_transfer(dev, NULL, buf, nbyte);

	return nbyte;
}

static ssize_t spi_write(struct char_dev *cdev, const void *buf, size_t nbyte) {
	struct spi_device *dev;

	assert(cdev);
	assert(buf);

	dev = (struct spi_device *)cdev;

	spi_transfer(dev, (void *)buf, NULL, nbyte);

	return nbyte;
}

static int spi_ioctl(struct char_dev *cdev, int request, void *data) {
	struct spi_transfer_arg *transfer_arg;
	struct spi_device *dev;

	assert(cdev);

	dev = (struct spi_device *)cdev;

	log_debug("request=0x%x data=%p", request, data);

	switch (request) {
	case SPI_IOCTL_CS:
		spi_select(dev, (int)(intptr_t)data);
		break;
	case SPI_IOCTL_TRANSFER:
		transfer_arg = data;
		spi_transfer(dev, transfer_arg->in, transfer_arg->out,
		    transfer_arg->count);
		break;
	case SPI_IOCTL_CS_MODE:
		dev->flags = (int)(intptr_t)data;
		break;
	}

	return 0;
}

const struct char_dev_ops __spi_cdev_ops = {
    .read = spi_read,
    .write = spi_write,
    .ioctl = spi_ioctl,
};
