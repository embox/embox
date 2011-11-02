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

extern int __netif_receive_skb(sk_buff_t *skb);

static int pnet_linux_rx(net_packet_t pack) {
	__netif_receive_skb(pack->skbuf);
	return NET_HND_SUPPRESSED;
}

PNET_PROTO_DEF("linux gate", pnet_linux_rx, NULL, NULL);
