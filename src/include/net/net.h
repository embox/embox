/**
 * @file
 * @brief
 *
 * @date 11.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_NET_H_
#define NET_NET_H_

#include <net/socket.h>
#include <stdbool.h>
#include <sys/socket.h>

#define NPROTO          PF_MAX /* a number of net protocols id in system */

struct sk_buff;

typedef struct net_proto_family {
	int		family;
	int		(*create)(struct socket *sock, int protocol);
} net_proto_family_t;

/**
 * Add a socket protocol handler
 * @param ops description of protocol
 * @return error code
 */
extern int sock_register(const struct net_proto_family *ops);

/**
 * Remove a protocol handler.
 * @param family protocol family to remove
 */
extern void sock_unregister(int family);

extern const struct net_proto_family * socket_repo_get_family(int family);

static inline bool is_a_valid_sock_type(int type){
	return ((type == SOCK_STREAM) ||
					(type == SOCK_DGRAM) ||
					(type == SOCK_RAW) ||
					(type == SOCK_SEQPACKET) ||
					(type == SOCK_PACKET));
}

static inline bool is_a_valid_family(int family){
	return ((family == AF_UNSPEC) ||
					(family == AF_UNIX) ||
					(family == AF_INET) ||
					(family == AF_PACKET));
}

#endif /* NET_NET_H_ */
