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

#include <embox/net/sock.h>

#include <framework/mod/options.h>
#include <net/l4/udp.h>
#include <net/lib/udp.h>
#include <net/sock.h>
#include <net/socket/inet_sock.h>
#include <embox/net/pack.h>

#include <util/indexator.h>
#include <util/list.h>

#define MODOPS_AMOUNT_UDP_PORT OPTION_GET(NUMBER, amount_udp_port)

static const struct sock_proto_ops udp_sock_ops_struct;
const struct sock_proto_ops *const udp_sock_ops
		= &udp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 1,
		udp_sock_ops_struct);

static int udp_sendmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	int ret;
	size_t data_len, total_len, actual_len;
	struct sk_buff *skb;
	const struct sockaddr_in *to;

	assert(sk != NULL);
	if (sk->o_ops == NULL) {
		return -ENOSYS;
	}

	assert(msg != NULL);
	assert(msg->msg_iov != NULL);
	data_len = msg->msg_iov->iov_len;
	total_len = actual_len = UDP_HEADER_SIZE + data_len;
	skb = NULL;

	assert(sk->o_ops->make_pack != NULL);
	ret = sk->o_ops->make_pack(sk,
			(const struct sockaddr *)msg->msg_name,
			&actual_len, &skb);
	if (ret != 0) {
		return ret;
	}
	else if (actual_len < total_len) {
		return -EMSGSIZE;
	}

	to = msg->msg_name != NULL
			? (const struct sockaddr_in *)msg->msg_name
			: (const struct sockaddr_in *)&to_inet_sock(sk)->dst_in;

	assert(skb != NULL);
	assert(skb->h.uh != NULL);
	udp_build(skb->h.uh, sock_inet_get_src_port(sk),
			to->sin_port, total_len);

	assert(msg->msg_iov->iov_base != NULL);
	memcpy(skb->h.uh + 1, msg->msg_iov->iov_base, data_len);

	udp4_set_check_field(skb->h.uh, skb->nh.iph);

	assert(sk->o_ops->snd_pack != NULL);
	return sk->o_ops->snd_pack(skb);
}

INDEX_CLAMP_DEF(udp_sock_port, 0, MODOPS_AMOUNT_UDP_PORT,
		IPPORT_RESERVED, IPPORT_USERRESERVED - 1);
static LIST_DEF(udp_sock_list);

static const struct sock_proto_ops udp_sock_ops_struct = {
	.sendmsg   = udp_sendmsg,
	.recvmsg   = sock_nonstream_recvmsg,
	.sock_port = &udp_sock_port,
	.sock_list = &udp_sock_list
};
