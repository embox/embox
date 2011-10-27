/**
 * @file
 * @brief Network connections
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>

int pnet_node_attach(net_node_t node, net_id_t id, net_node_t other) {
	if (node == NULL) {
		return -1;
	}

	switch (id) {
	case NET_RX_DFAULT:
		node->rx_dfault = other;
		break;
	case NET_TX_DFAULT:
		node->tx_dfault = other;
	}

	return 0;
}

net_node_t pnet_node_get(net_node_t node, net_id_t id) {
	switch (id) {
	case NET_RX_DFAULT:
		return node->rx_dfault;
	case NET_TX_DFAULT:
		return node->tx_dfault;
	}
	return NULL;
}
