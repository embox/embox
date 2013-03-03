/*
 * @file
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.11
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <string.h>

#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth/bluetooth.h>

#include <pnet/core.h>
#include <pnet/repo.h>
#include <pnet/node.h>
#include <pnet/pack/pack_alone.h>

#include <pnet/node/direct_comm.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(dc_pnet_init);

#define DC_BUFF_SIZE 0x20 /* lego_nxt direct command maximum length */

static int ctrl_rx(struct pnet_pack *pack);
static int data_rx(struct pnet_pack *pack);

PNET_NODE_DEF_NAME(PNET_NODE_DIRECT_COMM_FORMATION_DATA, this_data, {
	.rx_hnd = data_rx
});

PNET_NODE_DEF_NAME(PNET_NODE_DIRECT_COMM_FORMATION_CTRL, this_ctrl, {
	.rx_hnd = ctrl_rx
});


static int handle_size(uint8_t *buff) {
	return buff[0] + (buff[1] << 8);
}

static int (*data_hnd)(void *);
static int get_body(void *msg);
static int get_header(void *msg);

static int get_header(void *msg) {
	int size = handle_size((uint8_t *) msg);

	if (size > DC_BUFF_SIZE - MSG_SIZE_BYTE_CNT) {
		//TODO error length
	}
	data_hnd = get_body;
	bluetooth_read(size);
	return NET_HND_STOP_FREE;
}

static int get_body(void *msg) {
	data_hnd = get_header;
	bluetooth_read(MSG_SIZE_BYTE_CNT);
	return NET_HND_FORWARD_DEFAULT;
}

static int data_rx(struct pnet_pack *pack) {
	return data_hnd(pnet_pack_get_data(pack));
}

static int ctrl_rx(struct pnet_pack *pack) {
	int status = *((uint8_t *) pnet_pack_get_data(pack));
	if (0 != status) { /* if connected */
		data_hnd = get_header;
		bluetooth_read(MSG_SIZE_BYTE_CNT);
	}

	return NET_HND_STOP_FREE;
}

static int dc_pnet_init(void) {
	return 0;
}
