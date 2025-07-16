/**
 * @file
 *
 * @date Dec 2, 2024
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#include <drivers/eeprom.h>

extern int eeprom_dev_private_register(struct eeprom_dev *dev);

int eeprom_dev_register(struct eeprom_dev *dev, int bus_nr, uint16_t bus_addr) {
    int err;

    assert(dev);

    dev->eed_bus = bus_nr;
    dev->eed_bus_addr = bus_addr;

    err = eeprom_dev_private_register(dev);
    if (err) {
		log_error("failed to register char device for \"%s\"", dev->eed_name);   
        return err;
	}

    return 0;
}

int eeprom_dev_unregister(struct eeprom_dev *dev) {
    return 0;
}

int eeprom_dev_open(struct eeprom_dev *dev) {

    return 0;
}

int eeprom_dev_close(struct eeprom_dev *dev) {
    return 0;
}
