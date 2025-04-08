/**
 * @file spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#ifndef DRIVERS_SPI_CONTROLLER_H_
#define DRIVERS_SPI_CONTROLLER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/char_dev.h>

#include <lib/libds/array.h>
#include <util/macro.h>

#include <framework/mod/options.h>
#include <config/embox/driver/spi/core.h>

#define SPI_REGISTRY_SZ \
	OPTION_MODULE_GET(embox__driver__spi__core, NUMBER, spi_quantity)

struct spi_controller_ops;

struct spi_controller {
	struct char_dev cdev;

	uint32_t flags;
	bool is_master;
	int bits_per_word;

	struct spi_controller_ops *spi_ops;
	void *priv;
};

struct spi_controller_ops {
	int (*init)(struct spi_controller *dev);
	int (*select)(struct spi_controller *dev, int cs);
	int (*set_mode)(struct spi_controller *dev, bool is_master);
	int (*transfer)(struct spi_controller *dev, uint8_t *in, uint8_t *out, int cnt);
};

extern struct spi_controller *spi_ctrl_by_id(int id);
extern int spi_ctrl_id(struct spi_controller *dev);

extern const struct char_dev_ops __spi_cdev_ops;

/* Note: if you get linker error like "redefinition of 'spi_device0'"
 * then you should reconfig system so SPI bus indecies do not overlap */
 #define SPI_CONTROLLER_DEF(dev_name, spi_dev_ops, dev_priv, idx)         \
 struct spi_device MACRO_CONCAT(spi_device, idx) = {           \
	 .cdev = CHAR_DEV_INIT(MACRO_CONCAT(spi_device, idx).cdev, \
		 MACRO_STRING(dev_name), &__spi_cdev_ops),             \
	 .spi_ops = spi_dev_ops,                                   \
	 .priv = dev_priv,                                         \
 };                                                            \
 CHAR_DEV_REGISTER((struct char_dev *)&MACRO_CONCAT(spi_device, idx))


#endif /* DRIVERS_SPI_CONTROLLER_H_ */
