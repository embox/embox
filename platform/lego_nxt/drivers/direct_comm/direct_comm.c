/*
 * @file
 *
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.2011
 * @author Anton Kozlov
 */
#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <drivers/nxt_direct_comm.h>
#include <drivers/nxt_motor.h>

#include <drivers/bluetooth.h>
#include <drivers/nxt_sensor.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>


static enum {
	COMM_SIZE,
	COMM_TYPE,
	COMM_COMM,
	COMM_BODY,
	COMM_TELEGRAM,
	COMM_REPLY
} reader_state;

int reply_need = 0;

int command = 0;
int size = 0;
int type = 0;

EMBOX_UNIT_INIT(nxt_direct_comm_init);

static int nxt_direct_comm_init(void) {
	reader_state = COMM_SIZE;
	motor_start(&motors[0], 0, 360, NULL);
	motor_start(&motors[1], 0, 360, NULL);
	motor_start(&motors[2], 0, 360, NULL);
	nxt_sensor_conf_pass(&sensors[0], NULL);
	return 0;
}


static int handle_type(uint8_t *buff) {
	type = buff[0];
	if (buff[0] == 0x00 || buff[0] == 0x80) {
		reader_state = COMM_COMM;
	}

	if (!(buff[0] & 0x80)) {
		reply_need = 1;
	}

	return 0;
}

static void reply_handle(uint8_t status) {
	uint8_t reply[] = {0x02, 0x00, 0x00};
	reply[1] = command;
	reply[2] = status;
	reply_need = 0;
	nxt_bluetooth_write(reply, 3);
}

static int handle_comm(uint8_t *buff) {
	command = buff[0];
	reader_state = COMM_BODY;
	return 0;
}

#define EDGE 200

static int sensor_send(uint8_t sensor_id) {
	sensor_t *sens = &sensors[sensor_id];
	sensor_val_t tmp = 0;
	uint8_t out[16];
	memset(out, 0, 16);
	out[0] = 0x02;
	out[1] = command;
	out[4] = 0;
	tmp =  nxt_sensor_get_value(sens);
	out[8] = (tmp >> 8) & 0xff;
	out[9] = tmp & 0xff;
	out[14] = (tmp > EDGE ? 0 : 1);
	reply_need = 0;
	nxt_bluetooth_write(out, 16);
	return 0;
}

static int handle_body(uint8_t *buff) {
	reader_state = COMM_TYPE;
	switch (command) {
		uint8_t power;
		case DC_SET_OUTPUT_STATE:
			power = buff[1];
			if (buff[0] != 0xff) {
				motor_set_power(&motors[buff[0]], power);
			} else {
				motor_set_power(&motors[0], power);
				motor_set_power(&motors[1], power);
				motor_set_power(&motors[2], power);
			}
			return 0;
			break;
		case DC_GET_INPUT_VALUES:
			sensor_send(buff[0]);
			return 0;
			break;
		case DC_EX_SET_M_OUTPUT_STATE:
			motor_set_power(&motors[0], buff[0]);
			motor_set_power(&motors[1], buff[1]);
			motor_set_power(&motors[2], buff[2]);
			return 0;
			break;
		case DC_QREAL_HELLO:
			return 0x42;
			break;
		default:
			break;
	}
	return 0;
}

static int handle_size(uint8_t *buff) {
	size = buff[0] + (buff[1] << 8);
	return size;
}

int direct_comm_handle(uint8_t *buff) {
	int ret_val = 0;
	switch (reader_state) {
		uint8_t *cbuf;
		uint8_t status;
		case COMM_SIZE:
			reader_state = COMM_TELEGRAM;
			ret_val = handle_size(buff);
			break;
		case COMM_TELEGRAM:
			cbuf = buff;
			handle_type(cbuf);
			cbuf += 1;
			handle_comm(cbuf);
			cbuf += 1;
			status = handle_body(cbuf);
			if (reply_need) {
				reply_handle(status);
			}
			command = 0;
			reader_state = COMM_SIZE;
			ret_val = MSG_SIZE_BYTE_CNT;
			break;

		default:
			break;
	}
	return ret_val;
}

int direct_comm_init_read(void) {
	return MSG_SIZE_BYTE_CNT;
}


