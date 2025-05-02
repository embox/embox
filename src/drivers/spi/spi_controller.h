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
struct spi_device;

struct spi_controller {
	struct char_dev cdev;

	uint32_t flags;
	bool is_master;
	int bits_per_word;

	struct spi_controller_ops *spi_ops;
	void *priv;

	int                         spic_bus_num;
};

struct spi_controller_ops {
	int (*init)(struct spi_controller *dev);
	int (*select)(struct spi_controller *dev, int cs);
	int (*set_mode)(struct spi_controller *dev, bool is_master);
	int (*transfer)(struct spi_controller *dev, uint8_t *in, uint8_t *out, int cnt);
};

extern struct spi_controller *spi_controller_by_id(int id);
extern int spi_controller_id(struct spi_controller *dev);

extern const struct char_dev_ops __spi_cdev_ops;

 #define SPI_CONTROLLER_DEF(dev_name, spi_dev_ops, dev_priv, idx)         \
 	struct spi_controller MACRO_CONCAT(spi_controller, idx) = {           \
		.cdev = CHAR_DEV_INIT(MACRO_CONCAT(spi_controller, idx).cdev, \
			MACRO_STRING(dev_name), &__spi_cdev_ops),             \
		.spi_ops = spi_dev_ops,                                   \
		.priv = dev_priv,                                         \
		.spic_bus_num = idx,                                      \
 	};                                                            \
	CHAR_DEV_REGISTER((struct char_dev *)&MACRO_CONCAT(spi_controller, idx)); \
	ARRAY_SPREAD_DECLARE(struct spi_controller *, __spi_controller_registry);       \
	ARRAY_SPREAD_ADD(__spi_controller_registry, &MACRO_CONCAT(spi_controller, idx))

#endif /* DRIVERS_SPI_CONTROLLER_H_ */
