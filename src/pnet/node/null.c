/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>

static int net_null_tx_hnd(net_packet_t pack) {
	return NET_HND_SUPPRESSED;
}

static int net_null_rx_hnd(net_packet_t pack) {
	return NET_HND_SUPPRESSED;
}

static struct net_proto null_proto = {
	.tx_hnd = net_null_tx_hnd,
	.rx_hnd = net_null_rx_hnd
};

static struct net_node null_node = {
	.proto = &null_proto
};

net_node_t pnet_get_node_null(void) {
	return &null_node;
}
