/**
 * @file spi.c
 * @brief NOTE: When you use spi.core without spi.devfs
 * you should make sure device-specific stuff is initialized
 * somehow else
 *
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>

#include <drivers/char_dev.h>
#include <drivers/spi.h>

#include <embox/unit.h>

#include <framework/mod/options.h>

#define USE_GPIO_CS     OPTION_GET(BOOLEAN, use_cs)

ARRAY_SPREAD_DECLARE(struct spi_device *, __spi_device_registry);
ARRAY_SPREAD_DECLARE(struct spi_controller *, __spi_controller_registry);

EMBOX_UNIT_INIT(spi_init);

#if USE_GPIO_CS

#include <drivers/gpio.h>

static inline int spi_ctrl_pins_init(struct spi_controller *spi_c) {
	if (spi_c->spic_pins) {
		gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_port,
				(1 << spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_pin),
				GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_func) |
				GPIO_MODE_OUT_PUSH_PULL
				// GPIO_MODE_OUT
			);

		gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_port,
				(1 << spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_pin),
				GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_func) |
				GPIO_MODE_OUT_PUSH_PULL
				// GPIO_MODE_OUT
			);

		gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_RX_IDX].pd_port,
				(1 << spi_c->spic_pins[SPIC_PIN_RX_IDX].pd_pin),
				GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_RX_IDX].pd_func) |
				GPIO_MODE_IN_PULL_UP
				//GPIO_MODE_IN		
			);
	}
	return 0;
}

static inline int spi_dev_cs_pin_init(struct spi_device *dev) {
	(void) dev;
	return 0;
}

static inline int spi_dev_cs_control(struct spi_device *dev, int state) {
	const struct pin_description *cs_pin;
    
    if ( !dev || !dev->spid_cs_pin ) {
		log_debug("SPI_CS is not used");	        
        return 0;	// CS not use by software
    }
    
    cs_pin = dev->spid_cs_pin; 

    gpio_set(cs_pin->pd_port, (1 << cs_pin->pd_pin), state);
    
    return 0;
}

#else /* USE_GPIO_CS == false */

static inline int spi_ctrl_pins_init(struct spi_controller *spi_c) {
	(void) spi_c;
	return 0;
}

static inline int spi_dev_cs_pin_init(struct spi_device *dev) {
	(void) dev;
	return 0;
}

static inline int spi_dev_cs_control(struct spi_device *dev, int state) {
	(void) dev;
	(void) state;
	return 0;
}

#endif /* USE_GPIO_CS */

/**
 * @brief Call device-specific init handlers for all
 * registered SPI devices.
 *
 * @return Always 0
 */
static int spi_init(void) {
	struct spi_device *dev;
	struct spi_controller *cntl;

	array_spread_foreach(cntl, __spi_controller_registry) {
		assert(cntl);
		if (cntl->spic_ops && cntl->spic_ops->init) {

			cntl->spic_ops->init(cntl);
		}
	}

	array_spread_foreach(dev, __spi_device_registry) {
		assert(dev);

		cntl = spi_controller_by_id(dev->spid_bus_num);
		dev->spid_spi_cntl = cntl;

		spi_dev_cs_pin_init(dev);

		if (!dev->spid_ops) {
			continue;
		}

		if (dev->spid_ops->init) {
			dev->spid_ops->init(dev);
		}
		else {
			log_warning("SPI%d has no init funcion", dev->spid_bus_num);
		}
	}

	return 0;
}

/**
 * @brief Perform device-dependent SPI transfer operation
 *
 * @param dev SPI device
 * @param in  Data which is passed to controller
 * @param out Data which is received from controller
 * @param cnt Number of bytes to be passed
 *
 * @return Error code
 */
int spi_transfer(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt) {
	struct spi_controller *cntl;
	int res;

	assert(dev);
	assert(in || out);

	cntl = dev->spid_spi_cntl;
	
	cntl->spic_active_dev = dev; /* store an active SPI device in controller */

	if (dev->spid_flags & SPI_CS_ACTIVE /*&& spi_dev->is_master*/) {
		spi_dev_cs_control(dev, 0);	
		log_debug("SPI_CS_ACTIVE(%d::%d)", dev->spid_bus_num, dev->spid_idx);
	}

	if (cntl && cntl->spic_ops && cntl->spic_ops->transfer) {
		/** TODO: lock ??? */
		cntl->flags = dev->spid_flags;
		res = cntl->spic_ops->transfer(cntl, in, out, cnt);
		/** TODO: unlock ??? */
		goto out;
	}

	if (dev->spid_ops->transfer == NULL) {
		log_debug("Transfer operation is not supported for SPI%d", spi_dev_id(dev));
		res = -ENOSUPP;
	}

	res = dev->spid_ops->transfer(dev, in, out, cnt);

out:

	if (dev->spid_flags & SPI_CS_INACTIVE /*&& dev->is_master*/) {
		spi_dev_cs_control(dev, 1);
		log_debug("SPI_CS_INACTIVE(%d::%d)", dev->spid_bus_num, dev->spid_idx);
	}

	cntl->spic_active_dev = NULL; /* store an active SPI device in controller */

	return res;
}

/**
 * @brief Perform device-spefic chip select operation
 */
int spi_select(struct spi_device *dev, int cs) {
	struct spi_controller *cntl;

	assert(dev);

	cntl = dev->spid_spi_cntl;
	if (cntl && cntl->spic_ops && cntl->spic_ops->select) {
		return cntl->spic_ops->select(cntl, cs);
	}

	if (dev->spid_ops->select == NULL) {
		log_debug("Select operation is not supported for SPI%d", spi_dev_id(dev));
		return -ENOSUPP;
	}

	return dev->spid_ops->select(dev, cs);
}

/**
 * @brief Set device to master or slave mode
 */
static int spi_set_mode(struct spi_device *dev, bool is_master) {
	struct spi_controller *cntl;

	assert(dev);

	cntl = dev->spid_spi_cntl;
	if (cntl && cntl->spic_ops && cntl->spic_ops->set_mode) {
		return cntl->spic_ops->set_mode(cntl, is_master);
	}

	if (dev->spid_ops->set_mode == NULL) {
		log_debug("SPI mode setting is not supported for SPI%d", spi_dev_id(dev));
		return -ENOSUPP;
	}

	dev->spid_is_master = is_master;

	return dev->spid_ops->set_mode(dev, is_master);
}

int spi_set_master_mode(struct spi_device *dev) {
	return spi_set_mode(dev, true);
}

int spi_set_slave_mode(struct spi_device *dev) {
	return spi_set_mode(dev, false);
}
