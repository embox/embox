/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */
#include <stdio.h>
#include <pnet/core.h>
#include <pnet/node.h>

static int stamp = 1;
static void print_pack(net_packet_t pack) {
	printf("%d: ", stamp++);
	switch(pack->dir) {
		case NET_PACKET_RX:
			printf("RX");
			break;
		case NET_PACKET_TX:
			printf("TX");
			break;
	};

	printf(": %d bytes\n", pack->len);
}




static int net_info_tx_hnd(net_packet_t pack) {
	print_pack(pack);
	return NET_HND_DFAULT;
}

static int net_info_rx_hnd(net_packet_t pack) {
	print_pack(pack);
	return NET_HND_DFAULT;
}

static struct pnet_proto info_proto = {
	.tx_hnd = net_info_tx_hnd,
	.rx_hnd = net_info_rx_hnd
};

net_node_t pnet_get_node_info(void) {
	return (net_node_t) pnet_node_alloc(0, &info_proto);
}
