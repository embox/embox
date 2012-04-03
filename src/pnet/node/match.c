/**
 * @file
 * @brief
 *
 * @date 13.03.12
 * @author Alexander Kalmuk
 */


#include <mem/objalloc.h>

#include <pnet/node.h>
#include <pnet/pnet_pack.h>

#include <pnet/repo.h>
#include <pnet/netfilter/match_lin.h>
#include <pnet/core.h>
#include <pnet/types.h>

#define NET_NODES_CNT 0x10

OBJALLOC_DEF(matcher_nodes, struct net_node_matcher, NET_NODES_CNT);

int matcher_free(net_node_t node) {
	net_node_matcher_t m = (net_node_matcher_t) node;

	objfree(&matcher_nodes, m);

	return 0;
}

net_node_t matcher_alloc(void) {
	net_node_matcher_t matcher = objalloc(&matcher_nodes);

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return (net_node_t) matcher;
}
