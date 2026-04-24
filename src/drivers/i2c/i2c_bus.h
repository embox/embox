/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#ifndef DRIVERS_I2C_BUS_H_
#define DRIVERS_I2C_BUS_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <framework/mod/options.h>

#include <config/embox/driver/i2c_core.h>

#define I2C_BUS_MAX \
	OPTION_MODULE_GET(embox__driver__i2c_core, NUMBER, i2c_bus_max)

/* i2c_msg flags */
/* I2C_M_RD is guaranteed to be 0x0001! */
#define I2C_M_RD           0x0001 /* read data, from slave to master */
#define I2C_M_TEN          0x0010 /* this is a ten bit chip address */
#define I2C_M_RECV_LEN     0x0400 /* length will be first received byte */
#define I2C_M_NO_RD_ACK    0x0800 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK   0x1000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR 0x2000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART      0x4000 /* if I2C_FUNC_NOSTART */
#define I2C_M_STOP         0x8000 /* if I2C_FUNC_PROTOCOL_MANGLING */

struct i2c_msg {
	uint8_t *buf;   /* pointer to msg data */
	uint32_t len;   /* msg length  */
	uint16_t addr;  /* slave address */
	uint16_t flags; /* flags */
};

__BEGIN_DECLS

extern ssize_t i2c_bus_read(unsigned bus_id, uint16_t addr, uint8_t *buf,
    size_t len);

extern ssize_t i2c_bus_write(unsigned bus_id, uint16_t addr, uint8_t *buf,
    size_t len);

extern ssize_t i2c_bus_transfer(unsigned bus_id, struct i2c_msg *msgs,
    size_t num_msgs);

extern int i2c_bus_set_baudrate(unsigned bus_id, uint32_t baudrate);

extern uint8_t i2c_bus_get_mask();

__END_DECLS

#endif /* DRIVERS_I2C_BUS_H_ */
