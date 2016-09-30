/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.12.2013
 */

#include <net/sock.h>
#include <util/member.h>
#include <fs/xattr_list.h>
#include <kernel/task/resource/security.h>
#include <security/smac/smac.h>

#include <fs/idesc.h>

static struct xattr_list *sock_xattr_list(struct idesc *idesc) {
	struct sock *sock;

	assert(idesc);

	sock = member_cast_out(idesc, struct sock, idesc);

	return &sock->sock_xattr.xattr_list;
}

static int sock_getxattr(struct idesc *idesc, const char *name, void *value,
		size_t size) {
	return getxattr_generic(sock_xattr_list(idesc), name, value, size);
}

static int sock_setxattr(struct idesc *idesc, const char *name,
		const void *value, size_t size, int flags) {
	return setxattr_generic(sock_xattr_list(idesc), name, value, size, flags);
}

static int sock_listxattr(struct idesc *idesc, char *list, size_t size) {
	return listxattr_generic(sock_xattr_list(idesc), list, size);
}

static int sock_removexattr(struct idesc *idesc, const char *name) {
	return removexattr_generic(sock_xattr_list(idesc), name);
}

static const struct idesc_xattrops sock_xattrops = {
	.getxattr = sock_getxattr,
	.setxattr = sock_setxattr,
	.listxattr = sock_listxattr,
	.removexattr = sock_removexattr,
};

void sock_xattr_init(struct sock *sock) {
	sock->idesc.idesc_xattrops = &sock_xattrops;

	xattr_list_init(&sock->sock_xattr.xattr_list);
}
