/**
 * @file
 * @brief
 *
 * @date 26.11.2011
 * @author Anton Kozlov
 */

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <embox/unit.h>

#include <drivers/nxt/motor.h>
#include <drivers/nxt/sensor.h>
#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>

#include <pnet/node/direct_comm.h>

#include <kernel/prom_printf.h>

EMBOX_UNIT_INIT(node_dc_init);

static struct lego_dc_msg_full dc_out_msg;

static void reply_handle(uint8_t status, uint8_t cmd, int addit_len, struct lego_dc_msg_full *msg_full) {
	struct lego_dc_msg *body = &(msg_full->body);
	msg_full->len = addit_len + 3;
	body->type = 0x02;
	body->command = cmd;
	body->tail[0] = status;
	bluetooth_write((uint8_t *) msg_full, sizeof(msg_full->len) + 3 + addit_len);
}

static void handle_body(struct lego_dc_msg *msg, int *addit_len, uint8_t addit_msg[]) {
	switch (msg->command) {
	case DC_KEEP_ALIVE:
		reply_handle(1, msg->command, 0, &dc_out_msg);
	case DC_EX_SET_M_OUTPUT_STATE:
		nxt_motor_set_power(NXT_MOTOR_A, msg->tail[0]);
		nxt_motor_set_power(NXT_MOTOR_B, msg->tail[1]);
		nxt_motor_set_power(NXT_MOTOR_C, msg->tail[2]);
	}
}

static int dc_rx_hnd(net_packet_t pack) {
	struct lego_dc_msg *msg;
	int addit_len = 0;

	msg = (struct lego_dc_msg *) pnet_pack_get_data(pack);

	handle_body(msg, &addit_len, dc_out_msg.body.tail + 1);

	return NET_HND_SUPPRESSED;
}

static int node_dc_init(void) {
	return 0;
}

PNET_NODE_DEF(PNET_NODE_DIRECT_COMM_EXECUTER,  {
		.rx_hnd = dc_rx_hnd,
		.tx_hnd = NULL
	});
