/**
 * @file
 *
 * @date Dec 2, 2024
 * @author Anton Bondarev
 */

#include <drivers/eeprom.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(at24_init);

#define I2C_BUS_NR         OPTION_GET(NUMBER,i2c_bus_nr)
#define I2C_BUS_ADDR       OPTION_GET(NUMBER,i2c_bus_addr)

struct at24_dev {
    struct eeprom_dev eeprom_dev;
    int flags;
};

static struct at24_dev at24_dev;

static int at24_init(void) {
    int res;

    at24_dev.eeprom_dev.eed_name = "at24_eeprom";
    at24_dev.eeprom_dev.eed_io_limit = 1024;

    res = eeprom_dev_register(&at24_dev.eeprom_dev, I2C_BUS_NR, I2C_BUS_ADDR);

    return res;
}