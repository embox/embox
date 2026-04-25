/**
 * @file
 *
 * @date Apr 24, 2026
 * @author Dmitry Pilyuk
 */

#ifndef I2C_PRIV_H_
#define I2C_PRIV_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <drivers/i2c/i2c.h>
#include <lib/libds/array_spread.h>

struct i2c_bus;

struct i2c_bus_ops {
	int (*i2c_master_xfer)(const struct i2c_bus *bus, struct i2c_msg *msgs,
	    size_t num_msgs);
	int (*i2c_set_baudrate)(const struct i2c_bus *bus, uint32_t baudrate);
	int (*i2c_init)(const struct i2c_bus *bus);
	int (*i2c_deinit)(const struct i2c_bus *bus);
};

struct pin_description;

#define I2C_BUS_PIN_SCL 0
#define I2C_BUS_PIN_SDA 1

struct i2c_bus {
	const struct i2c_bus_ops *i2cb_ops;
	uintptr_t i2cb_label;
	const struct pin_description *i2cb_pins;
	unsigned i2cb_id;
	void *i2cb_priv;
};

__BEGIN_DECLS

extern int i2c_bus_register(const struct i2c_bus *bus);

extern int i2c_bus_unregister(unsigned bus_id);

extern const struct i2c_bus *i2c_bus_get(unsigned bus_id);

__END_DECLS

#define I2C_BUS_REGISTER(bus)                                         \
	ARRAY_SPREAD_DECLARE(const struct i2c_bus *, __i2c_bus_registry); \
	ARRAY_SPREAD_ADD(__i2c_bus_registry, bus)

#endif /* I2C_PRIV_H_ */
