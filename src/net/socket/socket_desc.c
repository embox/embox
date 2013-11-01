/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>

static struct socket_desc *socket_desc_alloc(void) {
	return NULL;
}

struct socket_desc *socket_desc_create(int domain, int type, int protocol) {
	return socket_desc_alloc();
}

void socket_desc_destroy(struct socket_desc *desc) {

}

void socket_desc_check_perm(struct socket_desc *desc) {

}
