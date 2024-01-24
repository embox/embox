/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdlib.h>

#include <lib/libds/dlist.h>
#include <util/err.h>

#include <mem/misc/pool.h>
#include <drivers/i2c/i2c.h>

#include <framework/mod/options.h>

static struct i2c_bus *i2c_bus_repo[I2C_BUS_MAX];

POOL_DEF(i2c_bus_pool, struct i2c_bus, I2C_BUS_MAX);

int i2c_bus_register(struct i2c_adapter *adap, int id, const char *bus_name) {
	struct i2c_bus *i2c_bus;

	if (id < 0 || id >= I2C_BUS_MAX) {
		return -EINVAL;
	}
	if (!bus_name) {
		return -EINVAL;
	}

	if (strlen(bus_name) > MAX_I2C_BUS_NAME) {
		return -ENAMETOOLONG;
	}

	if (i2c_bus_repo[id]) {
		return -EBUSY;
	}

	i2c_bus = pool_alloc(&i2c_bus_pool);
	if (!i2c_bus) {
		return -ENOMEM;
	}

	i2c_bus->i2c_adapter = adap;
	i2c_bus->id = id;

	strncpy(i2c_bus->name, bus_name, MAX_I2C_BUS_NAME - 1);
	i2c_bus->name[MAX_I2C_BUS_NAME - 1] = '\0';

	i2c_bus_repo[id] = i2c_bus;

	return 0;
}

int i2c_bus_unregister(int id) {

	if (id < 0 || id >= I2C_BUS_MAX) {
		return -EINVAL;
	}
	if (i2c_bus_repo[id]) {
		pool_free(&i2c_bus_pool, i2c_bus_repo[id]);
		i2c_bus_repo[id] = NULL;
	}

	return 0;
}

struct i2c_bus *i2c_bus_get(int id) {
	if (id < 0 || id >= I2C_BUS_MAX) {
		return err_ptr(EINVAL);
	}
	return i2c_bus_repo[id];
}

ssize_t i2c_bus_read(int id, uint16_t addr, uint8_t *ch, size_t sz) {
	struct i2c_msg msg = {
			.addr = addr,
			.buf = (uint8_t *)ch,
			.flags = I2C_M_RD,
			.len = sz
	};

	return i2c_bus_transfer(id, addr, &msg, 1);
}

ssize_t i2c_bus_write(int id, uint16_t addr, const uint8_t *ch, size_t sz) {
	struct i2c_msg msg = {
			.addr = addr,
			.buf = (uint8_t *)ch,
			.flags = 0,
			.len = sz
	};

	return i2c_bus_transfer(id, addr, &msg, 1);
}

int i2c_bus_transfer(int id, uint16_t addr, struct i2c_msg *msgs, int num) {
	struct i2c_bus *bus;
	int i;

	if (id < 0 || id >= I2C_BUS_MAX) {
		return -EINVAL;
	}
	bus = i2c_bus_get(id);
	if (err(bus) || bus == NULL) {
		return -EBUSY;
	}

	for (i = 0; i < num; i++) {
		msgs[i].addr = addr;
	}

	assert(bus->i2c_adapter);
	assert(bus->i2c_adapter->i2c_algo);
	assert(bus->i2c_adapter->i2c_algo->i2c_master_xfer);

	return bus->i2c_adapter->i2c_algo->i2c_master_xfer(bus->i2c_adapter, msgs, num);
}

int i2c_bus_set_baudrate(int id, uint32_t baudrate) {
	struct i2c_bus *bus;

	if (id < 0 || id >= I2C_BUS_MAX) {
		return -EINVAL;
	}

	bus = i2c_bus_get(id);
	if (err(bus) || bus == NULL) {
		return -EBUSY;
	}

	assert(bus->i2c_adapter);
	assert(bus->i2c_adapter->i2c_algo);

	if (!bus->i2c_adapter->i2c_algo->i2c_set_baudrate) {
		return -ENOSUPP;
	}

	return bus->i2c_adapter->i2c_algo->i2c_set_baudrate(bus->i2c_adapter, baudrate);
}
