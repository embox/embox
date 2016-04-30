/**
 * @file
 * @brief Implements udp socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "net_sock.h"
#include <embox/net/pack.h>

#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <net/lib/udp.h>
#include <net/sock.h>
#include <net/socket/inet_sock.h>

#include <util/dlist.h>

#include <stdlib.h>

static const struct sock_proto_ops udp_sock_ops_struct;
const struct sock_proto_ops *const udp_sock_ops
		= &udp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 1,
		udp_sock_ops_struct);

static int udp_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	size_t data_len, data_len_, actual_len, skb_len, iov_len;
	struct sk_buff *skb;
	const struct sockaddr_in *to;
	const struct sockaddr *sockaddr;
	int i, size, ret;
	unsigned char *udp_header;

	assert(sk);
	assert(sk->o_ops);
	assert(sk->o_ops->make_pack);
	assert(msg);
	assert(msg->msg_iov);
	assert(msg->msg_iov->iov_base);

	data_len = 0;
	for (i = 0; i < msg->msg_iovlen; i++) {
		data_len += msg->msg_iov[i].iov_len;
	}
	data_len_ = data_len;

	actual_len = UDP_HEADER_SIZE + data_len;

	skb = NULL;
	sockaddr = (const struct sockaddr *)msg->msg_name;

	while (data_len_ > 0) {
		ret = sk->o_ops->make_pack(sk, sockaddr, &actual_len, &skb);

		if (ret != 0) {
			return ret;
		}

		if (msg->msg_name != NULL) {
			to = (const struct sockaddr_in *)msg->msg_name;
		} else {
			to = (const struct sockaddr_in *)&to_inet_sock(sk)->dst_in;
		}

		assert(skb);
		assert(skb->h.uh);

		udp_header = (unsigned char *) (skb->h.uh + 1);
		size = sizeof(msg->msg_iov[i].iov_base[0]);

		iov_len = i = skb_len = 0;

		for (i = 0; i < msg->msg_iovlen; i++) {
			iov_len = msg->msg_iov[i].iov_len * size;

			if (skb_len + iov_len <= actual_len) {
				memcpy(udp_header + skb_len, msg->msg_iov[i].iov_base, iov_len);
				skb_len += iov_len;
			}
			else {
				break;
			}
		}

		udp_build(skb->h.uh, sock_inet_get_src_port(sk), to->sin_port,
				skb_len + UDP_HEADER_SIZE);

		data_len_ -= skb_len;
		actual_len = data_len_ + UDP_HEADER_SIZE;
		udp4_set_check_field(skb->h.uh, skb->nh.iph);

		assert(sk->o_ops->snd_pack);
		ret = sk->o_ops->snd_pack(skb);

		if (0 > ret) {
			return ret;
		}
	}

	return data_len;
}

static DLIST_DEFINE(udp_sock_list);

static int udp_fillmsg(struct sock *sk, struct msghdr *msg,
		struct sk_buff *skb) {
	struct sockaddr_in *inaddr;

	inaddr = (struct sockaddr_in *)msg->msg_name;

	inaddr->sin_family = AF_INET;
	memcpy(&inaddr->sin_addr, &ip_hdr(skb)->saddr, sizeof(in_addr_t));
	inaddr->sin_port = udp_hdr(skb)->source;

	return 0;
}

static const struct sock_proto_ops udp_sock_ops_struct = {
	.sendmsg   = udp_sendmsg,
	.recvmsg   = sock_dgram_recvmsg,
	.fillmsg   = udp_fillmsg,
	.sock_list = &udp_sock_list
};
