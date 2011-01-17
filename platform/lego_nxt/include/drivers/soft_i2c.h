/**
 * @file
 * @brief Soft I2C interface
 *
 * @date 12.12.2010
 * @author Anton Kozlov
 */

#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_

#include <drivers/pins.h>
#include <types.h>

typedef enum {
	START,
	READ_FALL,
	READ_RISE,
	READ_ACK_RISE,
	READ_ACK_FALL,
	READ_NOT_ACK_RISE,
	READ_NOT_ACK_FALL,
	WRITE_FALL,
	WRITE_RISE,
	WRITE_ACK_FALL,
	WRITE_ACK_RISE,
	STOP_DATA_FALL,
	WAIT,
	I2C_STOP,
	STOP_FALL,
	STOP_RISE,
	OFF,
	IDLE
} i2c_state_t;

enum operation_enum_t { WRITE, READ};
/* allow sharing common parts in i2c_state */

typedef struct {
	pin_mask_t sda;
	pin_mask_t scl;
	uint8_t	*data;
	uint32_t data_cnt;
	uint8_t bit_cnt;
	uint8_t write_byte;
	uint8_t wait;
	i2c_state_t state;
	enum operation_enum_t operation;
} i2c_port_t;

/**
 * Set @link port @endlink state to start. For proper use @link port->state @endlink
 * must be in @link IDLE @endlink state
 * @param port Port
 * @param addr 7-bit addres of device on bus
 * @param data Data will be read to
 * @param @count Size of data read
 */
void i2c_read(i2c_port_t *port, uint8_t addr, uint8_t *data, uint32_t count);
/**
 * Set @link port @endlink state to start. For proper use @link port->state @endlink
 * must be in @link IDLE @endlink state
 * @param port Port
 * @param addr 7-bit addres of device on bus
 * @param data Data to read
 * @param @count Size of data write
 */
void i2c_write(i2c_port_t *port, uint8_t addr, uint8_t *data, uint32_t count);
/**
 * Init i2c_port
 * @param port Port
 */
void i2c_init(i2c_port_t *port);
//typedef void (*i2c_done_callback)(i2c_port_t *i2c_port);

#define MAX_PORTS (sizeof(int))
//extern i2c_port_t *ports[MAX_PORTS];

#endif /*SOFT_I2C_H*/
