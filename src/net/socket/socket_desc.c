/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <net/socket/socket_desc.h>

static struct socket_desc *socket_desc_alloc(void) {
	return NULL;
}

struct socket_desc *socket_desc_create(int domain, int type, int protocol) {
	return socket_desc_alloc();
}

void socket_desc_destroy(struct socket_desc *desc) {

}

void socket_desc_check_perm(struct socket_desc *desc, int ops_type,
		struct socket_desc_param *param) {

}

struct socket_desc *socket_desc_get(int idx) {
	return NULL;
}

struct socket_desc *socket_desc_accept(struct sock *sk) {
	return socket_desc_alloc();
}
