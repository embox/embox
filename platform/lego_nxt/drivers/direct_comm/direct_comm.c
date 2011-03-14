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

static void reply_handle(void) {
	uint8_t reply[] = {0x02, 0x00, 0x00};
	reply[1] = command;
	reply_need = 0;
	nxt_bluetooth_write(reply, 3);
}

static int handle_comm(uint8_t *buff) {
	command = buff[0];
	reader_state = COMM_BODY;
	return 0;
}

static int handle_body(uint8_t *buff) {
	int i;
	reader_state = COMM_TYPE;
	switch (command) {
		int power;
		case DC_SET_OUTPUT_STATE:

			power = buff[1];
			if (power > 100) {
				power = 100;
			}
			if (power < -100) {
				power = -100;
			}

			if (buff[0] != 0xff) {
				motor_set_power(&motors[buff[0]], power);
			} else {
				motor_set_power(&motors[0], power);
				motor_set_power(&motors[1], power);
				motor_set_power(&motors[2], power);
			}
			break;
		case DC_EX_SET_M_OUTPUT_STATE:
#if 0
			for (i = 0; i < NXT_N_MOTORS; i++) {
				motor_set_power(&motors[i], buff[i]);
			}
#endif
			motor_set_power(&motors[0], buff[0]);
			motor_set_power(&motors[1], buff[1]);
			motor_set_power(&motors[2], buff[2]);
			break;
		default:
			break;
	}
	return 0;
}

static int handle_size(uint8_t *buff) {
	size = buff[1] + (buff[0] << 8);
	//TRACE("$S=%x;", size);
	return size;
}

int direct_comm_handle(uint8_t *buff) {
	int ret_val = 0;
	switch (reader_state) {
		uint8_t *cbuf;
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
			handle_body(cbuf);
			if (reply_need) {
				reply_handle();
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


