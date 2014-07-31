#include <stdlib.h>
#include <embox/unit.h>
#include <drivers/i2c.h>

EMBOX_UNIT_INIT(i2c_init);

// TODO: should be configured externally
#define I2C_MAX_BUSES 4

static struct i2c_bus i2c_buses[I2C_MAX_BUSES];
// spinlock declaration goes here;

int __register_i2c_bus(struct i2c_adapter *dev)
{
	int i;

	for (i = 0; i < I2C_MAX_BUSES; i++)
		if (i2c_buses[i].free_entry)
			break;

	if (i == I2C_MAX_BUSES)
		return 1;

	i2c_buses[i].free_entry = 0;

	i2c_buses[i].id = i;
	i2c_buses[i].adapter = dev;
	return 0;
}

int register_i2c_bus(struct i2c_adapter *dev)
{
	// FIXME: Acquire spinlock
	int ret = __register_i2c_bus(dev);
	// FIXME: Release spinlock
	return ret;
}

int unregister_i2c_bus(int bus_id)
{
	if (bus_id < 0 || bus_id >= I2C_MAX_BUSES)
		return 1;
	// FIXME: acquire spinlock
	// FIXME: release devices
	i2c_buses[bus_id].free_entry = 1;
	// FIXME: release spinlock
	return 0;
}

struct i2c_bus* get_i2c_bus(int bus_id)
{
	if (bus_id < 0 || bus_id >= I2C_MAX_BUSES)
		return NULL;
	return &i2c_buses[bus_id];
}

int enumerate_i2c_buses(int (*callback)(int, void*), void* cookie)
{
	int i;

	for (i = 0; i < I2C_MAX_BUSES; i++)
		if (!i2c_buses[i].free_entry)
		{
			int ret = callback(i, cookie);
			if (ret)
				return ret;
		}
	return 0;
}

int register_i2c_device(int bus_id, struct i2c_device *dev)
{
	struct i2c_bus *bus = get_i2c_bus(bus_id);

	if (!bus)
		return 1;

	// FIXME: acquire bus spinlock
	dev->next = bus->devices.next;
	bus->devices.next = dev;
	// FIXME: release bus spinlock

	return 0;
}

int enumerate_i2c_devices(int bus_id, int (*callback)(struct i2c_device*, void*), void *cookie)
{
	struct i2c_bus *bus = get_i2c_bus(bus_id);
	struct i2c_device *cur;

	if (!bus)
		return 1;

	cur = &bus->devices;

	// FIXME: acquire bus spinlock
	while (cur->next)
	{
		cur = cur->next;
		int ret = callback(cur, cookie);
		if (ret)
			return ret;
	}
	// FIXME: release bus spinlock

	return 0;
}

static int i2c_init(void)
{
	int i;

	for (i = 0; i < I2C_MAX_BUSES; i++)
		i2c_buses[i].free_entry = 1;

	return 0;
}
