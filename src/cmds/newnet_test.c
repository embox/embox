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

#include <embox/cmd.h>

EMBOX_CMD(newnet_test_cmd);

#define SOCKET_N 5

int newnet_test_cmd(int argc, char *argv[]) {
	net_socket_t sock = NULL;
	net_node_t dev = NULL;
	if (argc <= 1) {
		return -EINVAL;

	}

	dev = pnet_dev_get_entry();

	sock = pnet_socket_open(NET_RX_DFAULT, dev);

	//pnet_path_set_prior((net_node_t) sock, 5);

	sock->node.id = SOCKET_N;

	pnet_socket_send(sock, argv[1], strlen(argv[1]));

	return 0;
}
