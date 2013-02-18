/**
 * @file
 * @brief Implements udp socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

//#include <stdlib.h>
#include <types.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>

#include <sys/socket.h>
#include <net/ip.h>
#include <net/udp.h>

#include <net/route.h>
#include <net/inetdevice.h>
#include <embox/net/sock.h>



EMBOX_NET_SOCK(AF_INET, SOCK_DGRAM, IPPROTO_UDP, udp_prot, inet_dgram_ops, 0, true);


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

static int udp_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		size_t len) {
	struct sk_buff *skb;

	struct inet_sock *inet = inet_sk(sk);

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

	return ip_send_packet(inet, skb);
}

static int udp_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		size_t len, int noblock, int flags) {
	struct sk_buff *skb;

	skb = skb_queue_front(sk->sk_receive_queue);
	if (skb && skb->len > 0) {
		if (len > (ntohs(skb->h.uh->len) - UDP_HEADER_SIZE)) {
			len = ntohs(skb->h.uh->len) - UDP_HEADER_SIZE;
		}
		memcpy((void *) msg->msg_iov->iov_base,
				(void *) (skb->h.raw + UDP_HEADER_SIZE), len);
		skb_free(skb); /* FIXME `skb` may contains more data than `len` */
	} else {
		len = 0;
	}
	msg->msg_iov->iov_len = len;
	return len;
}

static void udp_hash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof udp_table / sizeof udp_table[0]; ++i) {
		if (udp_table[i] == NULL) {
			udp_table[i] = (struct udp_sock *)sk;
			break;
		}
	}
}

static void udp_unhash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof udp_table / sizeof udp_table[0]; ++i) {
		if (udp_table[i] == (struct udp_sock *) sk) {
			udp_table[i] = NULL;
			break;
		}
	}
}

static void udp_close(struct sock *sk, long timeout) {
	sk_common_release(sk);
}

static int udp_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	sock_queue_rcv_skb(sk, skb);
	return 0;
}

static int udp_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return ENOERR;
}

static int udp_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return ENOERR;
}

#if 0
int udp_disconnect(struct sock *sk, int flags) {
        return 0;
}
#endif

const struct proto udp_prot = {
	.name        = "UDP",
	.close       = udp_close,
	.sendmsg     = udp_sendmsg,
	.recvmsg     = udp_recvmsg,
	.backlog_rcv = udp_queue_rcv_skb,
	.hash        = udp_hash,
	.unhash      = udp_unhash,
	.setsockopt  = udp_setsockopt,
	.getsockopt  = udp_getsockopt,
	.obj_size    = sizeof(struct udp_sock),
};
