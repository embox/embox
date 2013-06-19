/**
 * @file
 * @brief Implements udp socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>
#include <net/if_ether.h>

#include <sys/socket.h>
#include <net/ip.h>
#include <net/udp.h>

#include <net/route.h>
#include <net/inetdevice.h>
#include <embox/net/sock.h>
#include <mem/misc/pool.h>
#include <util/array.h>

static const struct sock_ops udp_sock_ops_struct;
const struct sock_ops *const udp_sock_ops = &udp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 1, udp_sock_ops_struct);

static int rebuild_udp_header(sk_buff_t *skb, __be16 source,
		__be16 dest, size_t len) {
	udphdr_t *udph = skb->h.uh;
	udph->source = source;
	udph->dest = dest;
	udph->len = htons(len + UDP_HEADER_SIZE);
	udph->check = 0;
	//udph->check = ptclbsum((void *) udph, udph->len);
	return 0;
}

static int udp_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct sk_buff *skb;

	struct inet_sock *inet = inet_sk(sk);
	size_t len = msg->msg_iov->iov_len;

	/* FIXME if msg->msg_iov->iov_len more than ETHERNET_V2_FRAME_SIZE */
	skb = skb_alloc(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ UDP_HEADER_SIZE +
				    msg->msg_iov->iov_len);
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;
	skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE; // + inet->opt->optlen;
	skb->sk = sk;
	memcpy((void*)((unsigned int)(skb->h.raw + UDP_HEADER_SIZE)),
				(void *) msg->msg_iov->iov_base, msg->msg_iov->iov_len);
	/* Fill UDP header */
	rebuild_udp_header(skb, inet->sport, inet->dport, len);

	ip_send_packet(inet, skb);
	return 0;
}

POOL_DEF(udp_sock_pool, struct udp_sock, MODOPS_AMOUNT_UDP_SOCK);
static LIST_DEF(udp_sock_list);

static const struct sock_ops udp_sock_ops_struct = {
	.sendmsg   = udp_sendmsg,
	.recvmsg   = sock_nonstream_recvmsg,
	.sock_pool = &udp_sock_pool,
	.sock_list = &udp_sock_list
};
