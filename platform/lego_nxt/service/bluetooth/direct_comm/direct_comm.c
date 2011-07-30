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

static int reply_need = 0;

static int command = 0;
static int size = 0;
static int type = 0;

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

/*two part array */
static uint8_t out_size[2];
static uint8_t __attribute__ ((unused))  out_addit[16];

static void reply_handle(uint8_t status, int addit_len) {
	/* its special to write to out_addit */
	out_size[0] = (addit_len) & 0xff;
	out_size[1] = 0;
	out_size[2] = 0x02;
	out_size[3] = command;
	out_size[4] = status;
	reply_need = 0;
	bluetooth_write(out_size, 2 + addit_len);
}

static int handle_comm(uint8_t *buff) {
	command = buff[0];
	reader_state = COMM_BODY;
	return 0;
}

#define EDGE 200

static int sensor_send(uint8_t sensor_id, int &addit_len) {
	sensor_val_t sens_val =  nxt_sensor_get_val(nxt_get_sensor(sensor_id));
	memset(out_addit, 0, 16);

	out_addit[3] = sensor_id;
	out_addit[4] = 1;
	out_addit[5] = 1;
	out_addit[8] = (sens_val >> 8) & 0xff;
	out_addit[9] = sens_val & 0xff;
	out_addit[10] = out_addit[8];
	out_addit[11] = out_addit[9];
	out_addit[14] = (sens_val > EDGE ? 0 : 1);
	*addit_len = 16;
	return 0;
}

static int keep_alive_send(int *addit_len) {
	*addit_len = 4;
#ifdef LEGO_COMPATIBLE
	TRACE("Hi! I'm Lego =(\n");
	return 0;
#else
	TRACE("Hi! I'm Embox!\n");
	return 1;
#endif

}


static int handle_body(uint8_t *buff, int *addit_len) {
	reader_state = COMM_TYPE;
	uint8_t power;
	switch (command) {
	case DC_SET_OUTPUT_STATE:
		power = buff[1];
		if (buff[0] != 0xff) {
			nxt_motor_set_power(nxt_get_motor(buff[0]), power);
		} else {
			nxt_motor_set_power(NXT_MOTOR_A, power);
			nxt_motor_set_power(NXT_MOTOR_B, power);
			nxt_motor_set_power(NXT_MOTOR_C, power);
		}
		*addit_len = 0;
		return 0;
		break;
	case DC_GET_INPUT_VALUES:
		return sensor_send(buff[0], &addit_len);
		break;
	case DC_KEEP_ALIVE:
		return keep_alive_send(&addit_len);
		break;
	case DC_EX_SET_M_OUTPUT_STATE:
		nxt_motor_set_power(NXT_MOTOR_A, buff[0]);
		nxt_motor_set_power(NXT_MOTOR_B, buff[1]);
		nxt_motor_set_power(NXT_MOTOR_C, buff[2]);
		*addit_len = 0;
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
	int next_read_cnt = 0;
	uint8_t *cbuf;
	uint8_t status;
	int addit_len;
	if (msg == BT_DRV_MSG_CONNECTED) {
	    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	} else if (msg == BT_DRV_MSG_READ) {
	    switch (reader_state) {
	    case COMM_SIZE:
		    reader_state = COMM_TELEGRAM;
		    next_read_cnt = handle_size(buff);
		    break;
	    case COMM_TELEGRAM:
		    cbuf = buff;
		    handle_type(cbuf);
		    cbuf += 1;
		    handle_comm(cbuf);
		    cbuf += 1;
		    status = handle_body(cbuf, &addit_len);
		    if (reply_need) {
			    reply_handle(status, addit_len);
		    }
		    command = 0;
		    reader_state = COMM_SIZE;
		    next_read_cnt = MSG_SIZE_BYTE_CNT;
		    break;
	    default:
		    break;
	    }
	    bluetooth_read(direct_comm_buff, next_read_cnt);
	}
	return 0;
}
