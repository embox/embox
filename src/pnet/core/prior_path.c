/**
 * @file
 * @brief Utilities to work with pnet paths
 *
 * @date 20.10.2011
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 *         - add node_for_each
 */

#include <pnet/prior_path.h>

struct sock;

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

struct pnet_path *pnet_calc_netdev_path(struct net_device *netdev) {
	prior_table[prior_cnt++].own_mac = netdev->dev_addr;
	prior_cnt++;
	return NULL;
}

struct pnet_path *pnet_calc_chardev_path(char *dev_name) {
	prior_table[prior_cnt++].prior_level = 0x10;
	prior_cnt++;
	return NULL;
}

#if 0
static int decrease_prior_down(net_node_t node, net_prior_t prior);
static int node_for_each_decrease_prior(net_node_t node, net_prior_t prior);
static int node_for_each_increase_prior(net_node_t node, net_prior_t prior);

int pnet_path_set_prior(net_node_t node, net_prior_t prior) {
	if (node->prior <= prior) {
		return node_for_each_increase_prior(node, prior);
	}

	return node_for_each_decrease_prior(node, prior);
}

static int node_for_each_decrease_prior(net_node_t node, net_prior_t prior) {
	net_node_t cur_node = node;
	net_node_t node_child;

	cur_node->prior = prior;
	cur_node = cur_node->tx_dfault;
	while (cur_node != NULL) {
		cur_node->prior = 0;
		for (size_t i = 0; i < CHILD_CNT; i++) {
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

	if (node == NULL) {
		return 0;
	}

	for (size_t i = 0; i < CHILD_CNT; i++) {
		if (NULL != (node_child = node->children[i])) {
			if (node_child->prior <= prior) {
				break;
			}
			node_child->prior = prior;
			decrease_prior_down(node_child, prior);
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
		cur_node = cur_node->tx_dfault;
	}
	return 0;
}
#endif
