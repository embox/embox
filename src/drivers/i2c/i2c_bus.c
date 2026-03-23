/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <drivers/i2c/i2c.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>
#include <util/err.h>

EMBOX_UNIT_INIT(i2c_bus_registry_init);

ARRAY_SPREAD_DEF(const struct i2c_bus *, __i2c_bus_registry);

static const struct i2c_bus *i2c_bus_repo[I2C_BUS_MAX];

int i2c_bus_register(const struct i2c_bus *bus) {
	unsigned bus_id;
	int err;

	if (!bus || !bus->i2c_ops || !bus->i2c_ops->i2c_master_xfer) {
		return -EINVAL;
	}

	bus_id = bus->i2c_id;

	if (bus_id >= I2C_BUS_MAX) {
		return -EINVAL;
	}

	if (i2c_bus_repo[bus_id]) {
		return -EBUSY;
	}

	if (bus->i2c_ops->i2c_init) {
		err = bus->i2c_ops->i2c_init(bus);
		if (err) {
			return err;
		}
	}

	i2c_bus_repo[bus_id] = bus;

	return 0;
}

int i2c_bus_unregister(unsigned bus_id) {
	const struct i2c_bus *bus;
	int err;

	bus = i2c_bus_get(bus_id);
	if (!bus) {
		return -EINVAL;
	}

	if (bus->i2c_ops->i2c_deinit) {
		err = bus->i2c_ops->i2c_deinit(bus);
		if (err) {
			return err;
		}
	}

	i2c_bus_repo[bus_id] = NULL;

	return 0;
}

int i2c_bus_transfer(unsigned bus_id, struct i2c_msg *msgs, size_t num_msgs) {
	const struct i2c_bus *bus;

	if (!msgs) {
		return -EINVAL;
	}

	bus = i2c_bus_get(bus_id);
	if (!bus) {
		return -EINVAL;
	}

	return bus->i2c_ops->i2c_master_xfer(bus, msgs, num_msgs);
}

int i2c_bus_set_baudrate(unsigned bus_id, uint32_t baudrate) {
	const struct i2c_bus *bus;

	bus = i2c_bus_get(bus_id);
	if (!bus) {
		return -EINVAL;
	}

	if (!bus->i2c_ops->i2c_set_baudrate) {
		return -ENOSUPP;
	}

	return bus->i2c_ops->i2c_set_baudrate(bus, baudrate);
}

const struct i2c_bus *i2c_bus_get(unsigned bus_id) {
	if (bus_id >= I2C_BUS_MAX) {
		return NULL;
	}

	return i2c_bus_repo[bus_id];
}

ssize_t i2c_bus_read(unsigned bus_id, uint16_t addr, uint8_t *buf, size_t len) {
	struct i2c_msg msg = {
	    .buf = buf,
	    .len = len,
	    .addr = addr,
	    .flags = I2C_M_RD,
	};

	return i2c_bus_transfer(bus_id, &msg, 1);
}

ssize_t i2c_bus_write(unsigned bus_id, uint16_t addr, uint8_t *buf, size_t len) {
	struct i2c_msg msg = {
	    .buf = buf,
	    .len = len,
	    .addr = addr,
	    .flags = 0,
	};

	return i2c_bus_transfer(bus_id, &msg, 1);
}

static int i2c_bus_registry_init(void) {
	const struct i2c_bus *bus;
	int err;

	array_spread_foreach(bus, __i2c_bus_registry) {
		if ((err = i2c_bus_register(bus))) {
			return err;
		}
	}

	return 0;
}
