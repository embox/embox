/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */

#ifndef SOCKET_DESC_H_
#define SOCKET_DESC_H_


#include <sys/socket.h>

#include <fs/idesc.h>
#include <net/socket/socket_desc.h>

#define SOCKET_DESC_OPS_SOCKET   0
#define SOCKET_DESC_OPS_BIND     1
#define SOCKET_DESC_OPS_CONNECT  2
#define SOCKET_DESC_OPS_LISTEN   3
#define SOCKET_DESC_OPS_ACCEPT   4
#define SOCKET_DESC_OPS_SEND     5


struct socket_desc_param {
	const struct sockaddr *addr;
	socklen_t *addrlen;
	int flags;
};

struct socket_desc {
	struct idesc idesc;
	struct sock sk;
};

extern struct socket_desc *socket_desc_create(int domain, int type,
		int protocol);

extern void socket_desc_destroy(struct socket_desc *desc);

extern void socket_desc_check_perm(struct socket_desc *desc, int ops_type,
		struct socket_desc_param *param);

extern struct socket_desc *socket_desc_get(int idx);


#endif /* SOCKET_DESC_H_ */
