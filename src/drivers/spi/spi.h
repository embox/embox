/**
 * @file spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

#include <drivers/char_dev.h>
#include <util/array.h>
#include <util/macro.h>

struct spi_ops;

struct spi_device {
	int    cs;
	int    flags;
	struct dev_module *dev;
	struct spi_ops *spi_ops;
	void  *priv;
};

struct spi_ops {
	int (*init)(struct spi_device *dev);
	int (*select)(struct spi_device *dev, int cs);
	int (*transfer)(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt);
};

extern struct spi_device *spi_dev_by_id(int id);
extern int spi_dev_id(struct spi_device *dev);
extern int spi_transfer(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt);
extern int spi_select(struct spi_device *dev, int cs);

extern const struct idesc_ops spi_iops;

#define SPI_REGISTRY_SZ 4

/* Note: if you get linker error like "redefinition of 'spi_device0'"
 * then you should reconfig system so SPI bus indecies do not overlap */
#define SPI_DEV_DEF(dev_name, spi_dev_ops, dev_priv, idx) \
	struct spi_device MACRO_CONCAT(spi_device, idx) = { \
		.spi_ops = spi_dev_ops, \
		.priv    = dev_priv, \
	}; \
	CHAR_DEV_DEF(dev_name, NULL, &spi_iops, &MACRO_CONCAT(spi_device, idx)) \

/* IOCTL-related stuff */
#define SPI_IOCTL_CS       0x1
#define SPI_IOCTL_TRANSFER 0x2
#define SPI_IOCTL_CS_MODE  0x3

struct spi_transfer_arg {
	uint8_t *in, *out;
	ssize_t count;
};

/* CS modes */
#define SPI_CS_ACTIVE   (1 << 0)
#define SPI_CS_INACTIVE (1 << 1)

#endif /* DRIVERS_SPI_H_ */
