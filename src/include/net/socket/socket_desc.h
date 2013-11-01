/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */

#ifndef SOCKET_DESC_H_
#define SOCKET_DESC_H_

#include <fs/idesc.h>
#include <net/socket/socket_desc.h>

struct socket_desc {
	struct idesc idesc;
	struct sock sk;
};

extern struct socket_desc *socket_desc_create(int domain, int type, int protocol);

extern void socket_desc_destroy(struct socket_desc *);

extern void socket_desc_check_perm(struct socket_desc *);


#endif /* SOCKET_DESC_H_ */
