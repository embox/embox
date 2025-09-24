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
#include <util/math.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/net_sock.h>
#include <embox/net/pack.h>

#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <net/lib/udp.h>
#include <net/sock.h>
#include <net/socket/inet_sock.h>

#include <lib/libds/dlist.h>

#include <stdlib.h>

static const struct sock_proto_ops udp_sock_ops_struct;
const struct sock_proto_ops *const udp_sock_ops = &udp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 1,
		udp_sock_ops_struct);

static int iov_msg_len(struct iovec *iov, int iovlen) {
	int len = 0;

	for (int i = 0; i < iovlen; i++) {
		len += iov[i].iov_len;
	}

	return len;
}

int iov_stream_make_queue(size_t len, size_t hdr_size, struct sk_buff_head *queue,
		struct sock *sk, const struct sockaddr *sockaddr) {
	int to_alloc = len;

	skb_queue_init(queue);

	while (to_alloc > 0) {
		struct sk_buff *skb = NULL;
		size_t actual_len = to_alloc + hdr_size;

		int ret = sk->o_ops->make_pack(sk, sockaddr, &actual_len, &skb);
		if (ret < 0) {
			skb_queue_purge(queue);
			return ret;
		}
		skb_queue_push(queue, skb);
		to_alloc -= actual_len - hdr_size;
	}

	return len;
}

int iov_dgram_make_queue(size_t len, size_t hdr_size, struct sk_buff_head *queue,
		struct sock *sk, const struct sockaddr *sockaddr) {

	skb_queue_init(queue);

	struct sk_buff *skb = NULL;
	size_t actual_len = len + hdr_size;

	int ret = sk->o_ops->make_pack(sk, sockaddr, &actual_len, &skb);
	if (ret < 0) {
		return ret;
	}

	skb_queue_push(queue, skb);
	return actual_len - hdr_size;
}

static int skb_queue_iov(struct sk_buff_head *queue, const struct iovec *iov, int iovlen, size_t header_len) {
	struct sk_buff *skb = skb_queue_front(queue);
	int i_iov = 0;
	int skb_pos = 0, iov_pos = 0;
	int ret = 0;

	while (!skb_queue_end(skb, queue) && (i_iov < iovlen)) {
		const int to_copy = min(skb->len - skb_pos, iov[i_iov].iov_len - iov_pos);
		memcpy(skb->mac.raw + header_len + skb_pos, iov[i_iov].iov_base + iov_pos, to_copy);

		iov_pos += to_copy;
		skb_pos += to_copy;
		ret += to_copy;

		if (skb->len == skb_pos) {
			skb = skb_queue_next(skb);
			skb_pos = 0;
		}

		if (iov[i_iov].iov_len == iov_pos) {
			++i_iov;
			iov_pos = 0;
		}
	}

	return ret;
}

static int udp_get_udp_offset(struct sk_buff *skb) {
	return (unsigned char *) (skb->h.uh) - skb->mac.raw;
}

static int udp_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	const in_port_t sk_src = sock_inet_get_src_port(sk);
	const struct sockaddr_in *const addr_to = (const struct sockaddr_in *)
			(msg->msg_name ? msg->msg_name : &to_inet_sock(sk)->dst_in);
	struct sk_buff_head queue;
	int err;

	const size_t iov_data_len = iov_msg_len(msg->msg_iov, msg->msg_iovlen);

	// msg->msg_name could be NULL, sockaddr is OK to be NULL
	const struct sockaddr *sockaddr = (const struct sockaddr *)msg->msg_name;

	const int out_data_len = iov_dgram_make_queue(iov_data_len, UDP_HEADER_SIZE, &queue, sk, sockaddr);
	if (out_data_len < 0) {
		return out_data_len;
	}

	// FIXME there should be a better way to get offset
	const int skb_udp_offset = udp_get_udp_offset(queue.next);

	const int bytes_copied = skb_queue_iov(&queue, msg->msg_iov, msg->msg_iovlen, skb_udp_offset + UDP_HEADER_SIZE);
	if (bytes_copied != out_data_len) {
		return -1;
	}

	err = 0;
	for (struct sk_buff *skb = skb_queue_pop(&queue); skb; skb = skb_queue_pop(&queue)) {
		udp_build(skb->h.uh, sk_src, addr_to->sin_port, skb->len - skb_udp_offset);
		udp4_set_check_field(skb->h.uh, skb->nh.iph);
		err = sk->o_ops->snd_pack(skb);
		if (err < 0) {
			break;
		}
	}

	if (err < 0) {
		skb_queue_purge(&queue);
		return err;
	}

	assert(skb_queue_front(&queue) == NULL); // should be empty at this point

	return out_data_len;
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
