/**
 * @file
 *
 * @brief
 *
 * @date 20.10.2011
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 * 			- add node_for_each
 */

#include <errno.h>
#include <pnet/core.h>
#include <pnet/prior_path.h>

static void decrease_prior_down(net_node_t node, net_prior_t prior);
static int node_for_each_decrease_prior(net_node_t node, net_prior_t prior);
static int node_for_each_increase_prior(net_node_t node, net_prior_t prior);

int pnet_path_set_prior(net_node_t node, net_prior_t prior) {
	if (node->prior <= prior) {
		return node_for_each_increase_prior(node, prior);
	} else {
		return node_for_each_decrease_prior(node, prior);
	}
}

static int node_for_each_decrease_prior(net_node_t node, net_prior_t prior) {
	net_node_t cur_node = node;
	net_node_t node_child;

	cur_node->prior = prior;
	cur_node = cur_node->parent;
	while (cur_node != NULL) {
		cur_node->prior = 0;
		for (int i = 0; i < CHILD_CNT; i++) {
			if (NULL != (node_child = cur_node->children[i])) {
				if (cur_node->prior <= node_child->prior) {
					cur_node->prior = node_child->prior;
				}
			}
		}
		cur_node = cur_node->parent;
	}
	return decrease_prior_down(node, prior);
}

static int decrease_prior_down(net_node_t node, net_prior_t prior) {
	net_node_t node_child = node;

	if (node != NULL) {
		for (int i = 0; i < CHILD_CNT; i++) {
			if (NULL != (node_child = node->children[i])) {
				if (node_child->prior <= prior) {
					break;
				}
				node_child->prior = prior;
				decrease_prior_down(node_child, prior);
			}
		}
	}

	return 0;
}

static int node_for_each_increase_prior(net_node_t node, net_prior_t prior) {
	net_node_t cur_node = node;
	while (cur_node != NULL) {
		if (cur_node->prior >= prior) {
			break;
		}
		cur_node->prior = prior;
		cur_node = cur_node->parent;
	}
	return 0;
}

static int __net_core_receive(net_packet_t pack) {
	net_node_t node = pack->node;
	net_id_t res = NET_HND_DFAULT;

	if (node->proto->rx_hnd != NULL) {
		res = node->proto->rx_hnd(pack);
	}

	if (res == NET_HND_DFAULT) {
		pack->node = pack->node->dfault;
	} else if (res > 0) {
		pack->node = pack->node->children[res];
	}

	if (res != NET_HND_SUPPRESSED) {
		rx_thread_add(pack);
	}

	return 0;
}

static int __net_core_send(net_packet_t pack) {
	net_node_t node = pack->node;
	int res = NET_HND_DFAULT;

	if (node->proto->tx_hnd != NULL) {
		res = node->proto->tx_hnd(pack);
	}

	if (res == NET_HND_DFAULT) {
		pack->node = pack->node->parent;
		rx_thread_add(pack);
	}

	return ENOERR;
}

int pnet_process(net_packet_t pack) {
	if (pack->dir == NET_PACKET_RX) {
		return __net_core_receive(pack);
	}
	return __net_core_send(pack);
}

struct pnet_path *pnet_get_dev_prior(struct net_device *dev) {
	return NULL;
}

static struct pnet_path prior_table[0x10]; //TODO convert it to list or heap
static int prior_cnt = 0;

struct pnet_path *pnet_calc_socket_path(struct sock *sock) {
	prior_table[prior_cnt].prior_level = 0x10;
	prior_cnt++;
	return NULL;
}

struct _pnet_path *pnet_calc_netdev_path(struct net_device *netdev) {
	prior_table[prior_cnt++].own_mac = netdev->dev_addr;
	prior_cnt++;
	return NULL;
}

struct _pnet_path *pnet_calc_chardev_path(char *dev_name) {
	prior_table[prior_cnt++].prior_level = 0x10;
	prior_cnt++;
	return NULL;
}
