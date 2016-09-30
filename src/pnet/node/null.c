/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core/core.h>
#include <pnet/core/node.h>
#include <pnet/core/repo.h>
#include <pnet/pack/pnet_pack.h>

static int net_null_tx_hnd(struct pnet_pack *pack) {
	pnet_pack_destroy(pack);
	return NET_HND_STOP;
}

static int net_null_rx_hnd(struct pnet_pack *pack) {
	pnet_pack_destroy(pack);
	return NET_HND_STOP;
}

PNET_NODE_DEF("null node", {
	.tx_hnd = net_null_tx_hnd,
	.rx_hnd = net_null_rx_hnd
});
