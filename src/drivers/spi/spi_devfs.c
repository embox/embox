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
#include <fs/dvfs.h>
#include <util/log.h>

static ssize_t spi_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	struct spi_device *dev = idesc_to_dev_module(desc)->dev_priv;
	ssize_t ret_size = 0;

	assert(iov);
	assert(dev);

	for (int i = 0; i < cnt; i++) {
		assert(iov[i].iov_base);
		spi_transfer(dev, NULL, iov[i].iov_base, iov[i].iov_len);
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t spi_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	struct spi_device *dev = idesc_to_dev_module(desc)->dev_priv;
	ssize_t ret_size = 0;

	assert(dev);
	assert(iov);

	for (int i = 0; i < cnt; i++) {
		assert(iov[i].iov_base);
		spi_transfer(dev, iov[i].iov_base, NULL, iov[i].iov_len);

		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static int spi_ioctl(struct idesc *desc, int request, void *data) {
	struct spi_device *dev = idesc_to_dev_module(desc)->dev_priv;
	struct spi_transfer_arg *transfer_arg;
	assert(dev);

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

const struct idesc_ops spi_iops = {
    .open = char_dev_default_open,
    .close = char_dev_default_close,
    .fstat = char_dev_default_fstat,
    .id_readv = spi_read,
    .id_writev = spi_write,
    .ioctl = spi_ioctl,
};
