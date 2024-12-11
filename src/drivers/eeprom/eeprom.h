/**
 * @file
 *
 * @date Dec 2, 2024
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_EEPROM_H_
#define SRC_DRIVERS_EEPROM_H_

#include <stdint.h>

#include <drivers/i2c/i2c.h>

#include <drivers/char_dev.h>

struct eeprom_dev;

struct eeprom_dev_ops {
	int (*start)(struct eeprom_dev *dev);
	int (*stop)(struct eeprom_dev *dev);
};

/*describe eeprom device instance */
struct eeprom_dev {
	struct char_dev eed_cdev;
	struct idesc *eed_idesc;

	const struct eeprom_dev_ops *eed_ops;
	const char *eed_name; /* registered name /dev/eeprom/<name> */

	int eed_flags;
#define eeprom_DEV_OPENED (1 << 0)

	struct dlist_head eed_dev_link;  /* global device list */

	uint8_t eed_bus;
	uint16_t eed_bus_addr;

	void *eed_priv;

	int eed_offset;
	int eed_io_limit;

};

/**
 * @brief Register new eeprom device
 *
 * @param dev
 *
 * @return 0 on sucess
 */
extern int 
eeprom_dev_register(struct eeprom_dev *dev, int bus_nr, uint16_t bus_addr);


/**
 * @brief Register new eeprom device
 *
 * @param dev
 *
 * @return 0 on sucess
 */
extern int eeprom_dev_unregister(struct eeprom_dev *dev);

/**
 * @brief Lookup eeprom device by name
 *
 * @param name
 *
 * @return Pointer to eeprom device or NULL
 */
extern struct eeprom_dev *eeprom_dev_lookup(const char *name);

/**
 * @brief Open eeprom device, optionally trigger event callback on new events
 * avaible.
 *
 * @param dev
 *
 * @return 0 on success
 */
extern int eeprom_dev_open(struct eeprom_dev *dev);

/**
 * @brief Close eeprom device, callback will not be called (if any)
 *
 * @param dev
 *
 * @return
 */
extern int eeprom_dev_close(struct eeprom_dev *dev);

extern struct eeprom_dev *eeprom_dev_iterate(struct eeprom_dev *dev);

#endif /* SRC_DRIVERS_EEPROM_H_ */
