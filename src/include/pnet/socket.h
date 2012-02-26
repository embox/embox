/**
 * @file
 * @brief Network types defs
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#ifndef PNET_SOCKET_H_
#define PNET_SOCKET_H_

#include <pnet/types.h>

#define SOCK_BUF_LEN 0x20

typedef struct net_socket {
	struct net_node node;
	char buf[SOCK_BUF_LEN];
} *net_socket_t;

extern int pnet_socket_send(net_socket_t node, void *data, int len);

extern int pnet_socket_receive(net_socket_t  node, void *data, int len);

extern net_socket_t pnet_socket_open(net_id_t id, net_node_t parent);

#endif /* PNET_SOCKET_H_ */
