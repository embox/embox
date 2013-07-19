/**
 * @file
 *
 * @brief used with linux network stack, optionally routes packs to pnet
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <arpa/inet.h>
#include <pnet/prior_path.h>
#include <kernel/thread.h>
#include <net/skbuff.h>
#include <net/l0/net_rx.h>

#include <pnet/core.h>

#include <embox/unit.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>

static int pnet_linux_rx(struct pnet_pack *pack) {
	struct sk_buff *skb;

	skb = (struct sk_buff *) pack->data;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	net_rx(skb);

	return NET_HND_FORWARD_DEFAULT;
}

PNET_NODE_DEF("linux gate", {
	.rx_hnd = pnet_linux_rx,
	.tx_hnd = NULL
});
