/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.12.2013
 */

#ifndef NET_SOCKET_XATTR_H_
#define NET_SOCKET_XATTR_H_

struct sock;

#include <module/embox/net/sock_xattr_api.h>

struct sock_xattr;

extern void sock_xattr_init(struct sock *sock);

#endif /* NET_SOCKET_XATTR_H_ */

