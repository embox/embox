/**
 * @file
 * @brief Network protocol manager
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <pnet/core.h>

#define NET_PROTO_CNT 0x10

int pnet_proto_init(pnet_proto_t proto, net_id_t id, net_hnd rx, net_hnd tx) {
	proto->proto_id = id;
	proto->rx_hnd = rx;
	proto->tx_hnd = tx;
	return 0;
}

