/**
 * @file
 * @brief
 *
 * @date 26.11.2011
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <embox/unit.h>

#include <drivers/nxt/motor.h>
#include <drivers/nxt/sensor.h>
#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth/bluetooth.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pack/pack_alone.h>

#include <pnet/node/direct_comm.h>

#include "device_config.h"

#define DEVICE_CONFIG_DATA 0
#define EXECUTE_COMMAND 1
#define REQUEST_SENSOR_DATA 2

EMBOX_UNIT_INIT(node_dc_init);

static struct lego_dc_msg_full dc_out_msg;

static void handle_command(struct lego_dc_msg *msg, int *addit_len, uint8_t addit_msg[]) {
	switch (msg->command) {
	case DC_EX_SET_M_OUTPUT_STATE:
		nxt_motor_set_power(NXT_MOTOR_A, msg->tail[0]);
		nxt_motor_set_power(NXT_MOTOR_B, msg->tail[1]);
		nxt_motor_set_power(NXT_MOTOR_C, msg->tail[2]);
		break;
	}
}

static void send_config(char *msg) {
	int length;
	char buff[256];
	struct DeviceConfigurationMessage message;
	buff[0] = DEVICE_CONFIG_DATA;
	strcpy(message._id, "LEGO_NXT");
	message._id_len = 8;
	message._type = _LEGO_NXT;
	message._commands[0] = DC_EX_SET_M_OUTPUT_STATE;
	message._commands_len = 1;
	message._sensors[0] = 0x00;
	message._sensors_len = 1;

	length = DeviceConfigurationMessage_write_delimited_to(&message, buff, 1);

	bluetooth_write((uint8_t *)buff, length);
}

static int dc_rx_hnd(struct pnet_pack *pack) {
	struct lego_dc_msg *lego_msg;
	int addit_len = 0;
	char *msg;

	msg = pnet_pack_get_data(pack);

	switch (msg[0]) {
		case DEVICE_CONFIG_DATA:
			send_config(msg);
			break;
		case EXECUTE_COMMAND:
			lego_msg = (struct lego_dc_msg *) (msg);
			handle_command(lego_msg, &addit_len, dc_out_msg.body.tail + 1);
			break;
		case REQUEST_SENSOR_DATA:
			break;
	}

	return NET_HND_STOP_FREE;
}

static int node_dc_init(void) {
	return 0;
}

PNET_NODE_DEF(PNET_NODE_DIRECT_COMM_EXECUTER,  {
		.rx_hnd = dc_rx_hnd,
		.tx_hnd = NULL
	});
