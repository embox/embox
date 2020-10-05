#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <config/embox/driver/i2c.h>

#define I2C_BUS_MAX   \
	OPTION_MODULE_GET(embox__driver__i2c, NUMBER, i2c_bus_max)

#define MAX_I2C_BUS_NAME 8

struct i2c_algorithm;

struct i2c_msg {
	uint16_t addr;           /* slave address */
	uint16_t flags;
	/* I2C_M_RD is guaranteed to be 0x0001! */
#define I2C_M_RD           0x0001 /* read data, from slave to master */
#define I2C_M_TEN          0x0010 /* this is a ten bit chip address */
#define I2C_M_RECV_LEN     0x0400 /* length will be first received byte */
#define I2C_M_NO_RD_ACK    0x0800 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK   0x1000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR 0x2000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART      0x4000 /* if I2C_FUNC_NOSTART */
#define I2C_M_STOP         0x8000 /* if I2C_FUNC_PROTOCOL_MANGLING */
	uint16_t len;           /* msg length  */
	uint8_t *buf;           /* pointer to msg data */
};

struct i2c_adapter {
	void *i2c_algo_data;
	const struct i2c_algorithm *i2c_algo; /* the algorithm to access the bus */
};

struct i2c_algorithm {
	int (*i2c_master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,
			int num);
	int (*i2c_set_baudrate)(struct i2c_adapter *adap, uint32_t baudrate);
};

struct i2c_bus {
	int id;
	char name[MAX_I2C_BUS_NAME];

	struct i2c_adapter *i2c_adapter;
};

extern int i2c_bus_register(struct i2c_adapter *adap, int id,
		const char *bus_name);

extern int i2c_bus_unregister(int bus_id);

extern struct i2c_bus *i2c_bus_get(int id);

extern ssize_t i2c_bus_read(int id, uint16_t addr, uint8_t *ch, size_t sz);

extern ssize_t i2c_bus_write(int id, uint16_t addr, const uint8_t *ch,
		size_t sz);

extern ssize_t i2c_bus_transfer(int id, uint16_t addr, struct i2c_msg *msgs,
		int num);

extern int i2c_bus_set_baudrate(int id, uint32_t baudrate);

#endif
