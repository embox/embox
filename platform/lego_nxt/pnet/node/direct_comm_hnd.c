/**
 * @file
 * @brief
 *
 * @date 28.10.2011
 * @author Anton Kozlov
 */

#include <types.h>
#include <hal/reg.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>

#include <drivers/nxt/direct_comm.h>
#include <drivers/nxt/motor.h>
#include <drivers/nxt/sensor.h>

#include <drivers/bluetooth.h>

#include <embox/unit.h>

#include <pnet/core.h>
#include <pnet/node.h>

#include <kernel/diag.h>

EMBOX_UNIT_INIT(node_dc_init);

#define ADDIT_MAX_LEN 62

static struct direct_comm_msg dc_out_msg;

static int reply_need(struct direct_comm_msg *dc) {
	if (dc->type & 0x80) {
		return 0;
	}
	return 1;
}

#include <kernel/prom_printf.h>

static void reply_handle(uint8_t status, uint8_t cmd, int addit_len, struct direct_comm_msg *dc_out_msg) {
	int extra_len = sizeof(dc_out_msg->type) +
		sizeof(dc_out_msg->command) + sizeof(dc_out_msg->addit_msg[0]); // 3
	dc_out_msg->len = addit_len + extra_len;
	dc_out_msg->type = 0x02;
	dc_out_msg->command = cmd;
	dc_out_msg->addit_msg[0] = status;
	bluetooth_write((uint8_t *) dc_out_msg, sizeof(dc_out_msg->len) + extra_len + addit_len);
}

#define EDGE 200

static int sensor_send(uint8_t sensor_id, int *addit_len, uint8_t addit_msg[]) {
	sensor_val_t sens_val =  nxt_sensor_get_val(nxt_get_sensor(sensor_id));
	memset(addit_msg, 0, 16);

	addit_msg[3] = sensor_id;
	addit_msg[4] = 1;
	addit_msg[5] = 1;
	addit_msg[8] = (sens_val >> 8) & 0xff;
	addit_msg[9] = sens_val & 0xff;
	addit_msg[10] = addit_msg[8];
	addit_msg[11] = addit_msg[9];
	addit_msg[14] = (sens_val > EDGE ? 0 : 1);
	*addit_len = 16;
	return 0;
}

static int keep_alive_send(int *addit_len, uint8_t addit_msg[]) {
	*addit_len = 0;
#ifdef LEGO_COMPATIBLE
	printf("Hi! I'm Lego =(\n");
	return 0;
#else
	printf("Hi! I'm Embox!\n");
	return 1;
#endif

}

static int handle_body(uint8_t command, uint8_t *buff, int *addit_len, uint8_t addit_msg[]) {
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
	case DC_GET_INPUT_VALUES:
		return sensor_send(buff[0], addit_len, addit_msg);
	case DC_KEEP_ALIVE:
		return keep_alive_send(addit_len, addit_msg);
	case DC_EX_SET_M_OUTPUT_STATE:
		nxt_motor_set_power(NXT_MOTOR_A, buff[0]);
		nxt_motor_set_power(NXT_MOTOR_B, buff[1]);
		nxt_motor_set_power(NXT_MOTOR_C, buff[2]);
		*addit_len = 0;
		return 0;
	default:
		return 0;
	}
}

static int dc_rx_hnd(net_packet_t pack) {
	struct direct_comm_msg *dc = (struct direct_comm_msg *) pack->skbuf->data;
	int addit_len = 0;
	int status = handle_body(dc->command, dc->addit_msg, &addit_len, dc_out_msg.addit_msg + 1);

	if (reply_need(dc)) {
	    reply_handle(status, dc->command, addit_len, &dc_out_msg);
	}

	pnet_pack_free(pack);
	return NET_HND_SUPPRESSED;
}

static struct pnet_proto dc_proto = {
//	.tx_hnd = dc_tx_hnd,
	.rx_hnd = dc_rx_hnd
};

net_node_t pnet_get_node_direct_comm(void) {
	return pnet_node_alloc(0, &dc_proto);
}

static int node_dc_init(void) {
	net_node_t devs = pnet_dev_get_entry();
	net_node_t dc_hnd = pnet_get_node_direct_comm();

	pnet_node_attach(devs, NET_RX_DFAULT, dc_hnd);

	return 0;
}
