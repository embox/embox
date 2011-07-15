/**
 * @file
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <drivers/nxt/direct_comm.h>
#include <drivers/nxt/motor.h>
#include <drivers/bluetooth.h>
#include <drivers/nxt/sensor.h>
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

#define DC_BUFF_SIZE 40

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

static int direct_comm_handle(int msg, uint8_t *buff);

static int nxt_direct_comm_init(void) {
	reader_state = COMM_SIZE;
	nxt_sensor_conf_pass(NXT_SENSOR_1, NULL);
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_comm_handle);
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
	bluetooth_write(reply, 3);
}

static int handle_comm(uint8_t *buff) {
	command = buff[0];
	reader_state = COMM_BODY;
	return 0;
}

#define EDGE 200

static int sensor_send(uint8_t sensor_id) {
	nxt_sensor_t *sens = &sensors[sensor_id];
	sensor_val_t tmp = 0;
	uint8_t out[16];
	memset(out, 0, 16);
	out[0] = 0x02;
	out[1] = command;
	out[4] = 0;
	tmp =  nxt_sensor_get_val(sens);
	out[8] = (tmp >> 8) & 0xff;
	out[9] = tmp & 0xff;
	out[14] = (tmp > EDGE ? 0 : 1);
	reply_need = 0;
	bluetooth_write(out, 16);
	return 0;
}

static int handle_body(uint8_t *buff) {
	reader_state = COMM_TYPE;
	uint8_t power;
	switch (command) {
	case DC_SET_OUTPUT_STATE:
		power = buff[1];
		if (buff[0] != 0xff) {
			nxt_motor_set_power(&nxt_motors[buff[0]], power);
		} else {
			nxt_motor_set_power(NXT_MOTOR_A, power);
			nxt_motor_set_power(NXT_MOTOR_B, power);
			nxt_motor_set_power(NXT_MOTOR_C, power);
		}
		return 0;
		break;
	case DC_GET_INPUT_VALUES:
		sensor_send(buff[0]);
		return 0;
		break;
	case DC_EX_SET_M_OUTPUT_STATE:
		nxt_motor_set_power(NXT_MOTOR_A, buff[0]);
		nxt_motor_set_power(NXT_MOTOR_B, buff[1]);
		nxt_motor_set_power(NXT_MOTOR_C, buff[2]);
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

static int direct_comm_handle(int msg, uint8_t *buff) {
	int ret_val = 0;
	uint8_t *cbuf;
	uint8_t status;
	if (msg == BT_DRV_MSG_CONNECTED) {
	    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	} else if (msg == BT_DRV_MSG_READ) {
	    switch (reader_state) {
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
	    bluetooth_read(direct_comm_buff, ret_val);
	}
	return 0;
}
