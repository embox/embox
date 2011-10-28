/*
 * @file
 * @brief
 *
 * @date 20.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <string.h>

#include <pnet/socket.h>
#include <pnet/core.h>

#include <framework/example/self.h>

EMBOX_EXAMPLE(socket_ex);

#define SOCKET_N 5

extern struct net_device *loopback_dev;

static int socket_ex(int argc, char *argv[]) {
	net_socket_t sock = NULL;
	net_node_t dev = NULL;
	net_node_t loopback = NULL;
	if (argc <= 1) {
		return 0;

	}

	dev = pnet_dev_get_entry();

	loopback = pnet_dev_register(loopback_dev);

	pnet_node_attach(dev, NET_TX_DFAULT, loopback);

	sock = pnet_socket_open(NET_TX_DFAULT, dev);

	pnet_node_attach(dev, NET_RX_DFAULT, (net_node_t) sock);

	//pnet_path_set_prior((net_node_t) sock, 5);

	sock->node.id = SOCKET_N;

	pnet_socket_send(sock, argv[1], strlen(argv[1]));

	return 0;
}
