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

typedef void (*i2c_done_callback)(i2c_port_t *i2c_port);

#define MAX_PORTS (sizeof(int))
extern i2c_port_t *ports[MAX_PORTS];

#endif /*SOFT_I2C_H*/
