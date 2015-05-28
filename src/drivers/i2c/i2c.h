#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

struct i2c_device {
	struct i2c_device *next;
};

struct i2c_adapter {
	void *priv;
};

struct i2c_bus {
	int id;
	int free_entry;
	struct i2c_adapter *adapter;
	struct i2c_device devices;
};

int register_i2c_bus(struct i2c_adapter*);
int unregister_i2c_bus(int bus_id);
struct i2c_bus* get_i2c_bus(int);
int enumerate_i2c_buses(int (*)(int, void*), void*);

int register_i2c_device(int, struct i2c_device*);
int enumerate_i2c_devices(int, int (*)(struct i2c_device*, void*), void*);

#endif
