/**
 * @file
 *
 * @brief used with linux network stack, optionally routes packs to pnet
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <net/in.h>
#include <pnet/prior_path.h>
#include <kernel/thread/api.h>
#include <net/skbuff.h>
#include <net/netdevice.h>

#include <pnet/core.h>

#include <embox/unit.h>
#include <pnet/repo.h>

EMBOX_UNIT_INIT(pnet_linux_init);

static struct pnet_proto linux_out;

extern int __netif_receive_skb(sk_buff_t *skb);

static int pnet_linux_rx(net_packet_t pack) {
	__netif_receive_skb(pack->skbuf);
	return NET_HND_SUPPRESSED;
}

static struct net_node lin_gate = {
	.proto = &linux_out
};

net_node_t pnet_get_node_linux_gate(void) {
	return &lin_gate;
}

static int pnet_linux_init(void) {
	int res = pnet_proto_init(&linux_out, 0, pnet_linux_rx, NULL);

	return res;
}

PNET_MODULE("linux gate", lin_gate);
