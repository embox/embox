/*
 * @file
 * @brief
 *
 * @date 31.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <string.h>

#include <pnet/socket.h>
#include <pnet/core.h>

EMBOX_EXAMPLE(socket_ex);



static int socket_ex(int argc, char *argv[]) {
	struct sock sock;
	struct pnet_graph graph;
	struct pnet_node node, src;

	net_node_t dev = NULL;

	if (sock = socket(PNET_GRAPH, 0, 0)) {
		//error
	}

	listen(sock);

	pnet_sock_set_prior(sock, 50);

	return 0;
}
