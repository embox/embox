/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <kernel/prom_printf.h>

static int net_null_tx_hnd(net_packet_t pack) {

	pnet_pack_free(pack);
	return NET_HND_SUPPRESSED;
}

static int net_null_rx_hnd(net_packet_t pack) {
	pnet_pack_free(pack);
	return NET_HND_SUPPRESSED;
}

PNET_NODE_DEF("null node", {
	.tx_hnd = net_null_tx_hnd,
	.rx_hnd = net_null_rx_hnd
});
