/**
 * @file
 * @brief
 *
 * @date 20.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <net/core.h>
#include <string.h>

#include <embox/cmd.h>

EMBOX_CMD(newnet_test_cmd);

extern net_node_t net_loopback_dev_get(void);

#define SOCKET_N 5

int newnet_test_cmd(int argc, char *argv[]) {
	net_socket_t sock = NULL;

	if (argc <= 1) {
		return -EINVAL;

	}

	sock = net_socket_open(SOCKET_N, net_loopback_dev_get());

	net_core_send((net_node_t) sock, argv[1], strlen(argv[1]));

	return 0;
}
