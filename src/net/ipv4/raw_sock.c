/**
 * @file
 * @brief Implementes raw socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

#include <net/protocol.h>
#include <net/sock.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/raw.h>
#include <net/inet_common.h>
#include <string.h>
#include <embox/net/sock.h>
#include <errno.h>

static struct proto raw_prot;
static const struct proto_ops inet_sockraw_ops;

EMBOX_NET_SOCK(SOCK_RAW, IPPROTO_IP, raw_prot, inet_sockraw_ops, 0, NULL);

static raw_sock_t *raw_hash[CONFIG_MAX_KERNEL_SOCKETS];

static int raw_rcv_skb(struct sock * sk, sk_buff_t * skb);

static int raw_init(struct sock *sk) {
	return 0;
}

#if 0
static struct sock *raw_lookup(__u8 proto) {
	struct sock *sk;
	size_t i;
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		sk = (struct sock*) raw_hash[i];
		if (sk && sk->sk_protocol == proto) {
			return sk;
		}
	}
	return NULL;
}
#endif


int raw_rcv(sk_buff_t *skb) {
	size_t i;
	struct sock *sk;
	iphdr_t *iph;
	sk_buff_t *cloned;

	iph = ip_hdr(skb);
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		sk = (struct sock *)raw_hash[i];
		if (sk && sk->sk_protocol == iph->proto) {
			cloned = skb_clone(skb, 0); // TODO without skb_clone()
			if(!cloned){
				printk("raw_sock.c: raw_rcv: couldn't clone socket buffer\n");
				return ENOMEM;
			}
			if (raw_rcv_skb(sk, cloned) < 0) {
				kfree_skb(cloned);
			}
		}
	}

	return ENOERR;
}

void raw_err(sk_buff_t *skb, uint32_t info) {
	size_t i;
	struct sock *sk;

	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		sk = (struct sock *)raw_hash[i];
		if (sk && sk->sk_protocol == ICMP_PROTO_TYPE) {
			sk->sk_err = info;
		}
	}
}

static void raw_close(struct sock *sk, long timeout) {
	/*TODO: release socket*/
	sk_free(sk);
}

static int raw_rcv_skb(struct sock *sk, sk_buff_t *skb) {
	if (sock_queue_rcv_skb(sk, skb) < 0) {
		return NET_RX_DROP;
	}
	return NET_RX_SUCCESS;
}

 static void raw_v4_hash(struct sock *sk) {
	size_t i;
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if (raw_hash[i] == NULL) {
			raw_hash[i] = (raw_sock_t *) sk;
			break;
		}
	}
}

static void raw_v4_unhash(struct sock *sk) {
	size_t i;
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if (raw_hash[i] == (raw_sock_t *) sk) {
			raw_hash[i] = NULL;
			break;
		}
	}
}

static int raw_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	struct inet_sock *inet = inet_sk(sk);
	sk_buff_t *skb = alloc_skb(ETH_HEADER_SIZE + len, 0);
	memcpy((void*)((unsigned int)(skb->data + ETH_HEADER_SIZE)),
					(void*) msg->msg_iov->iov_base, len);
	skb->h.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE +
			IP_MIN_HEADER_SIZE;// + inet->opt->optlen;
	skb->sk = sk;
	return ip_send_packet(inet, skb);
}

static int raw_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len) {
	struct sk_buff *skb;

	skb = skb_recv_datagram(sk, flags, 0, 0);
	if (skb && (skb->len > 0)) {
		if (len > (skb->len - ETH_HEADER_SIZE)) {
			len = skb->len - ETH_HEADER_SIZE;
		}
		memcpy((void*) msg->msg_iov->iov_base,
				(void*) (skb->data + ETH_HEADER_SIZE), len);
		kfree_skb(skb);
	} else {
		len = 0;
	}
	msg->msg_iov->iov_len = len;
	return len;
}

static int raw_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len) {
	struct inet_sock *inet = inet_sk(sk);
	struct sockaddr_in *addr = (struct sockaddr_in *) uaddr;
	inet->rcv_saddr = inet->saddr = addr->sin_addr.s_addr;
	return 0;
}

static int raw_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return 0;
}

static int raw_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return 0;
}

static int raw_ioctl(struct sock *sk, int cmd, unsigned long arg) {
	return 0;
}

int ip4_datagram_connect(struct sock *sk,
				struct sockaddr *uaddr, int addr_len) {
	return 0;
}

static struct proto raw_prot = {
	.name = "RAW",
	.close = raw_close,
	.connect = ip4_datagram_connect,
	.disconnect = udp_disconnect,
	.ioctl = raw_ioctl,
	.init = raw_init,
	.setsockopt = raw_setsockopt,
	.getsockopt = raw_getsockopt,
	.sendmsg = raw_sendmsg,
	.recvmsg = raw_recvmsg,
	.bind = raw_bind,
	.backlog_rcv = raw_rcv_skb,
	.hash = raw_v4_hash,
	.unhash = raw_v4_unhash,
	.obj_size = sizeof(struct raw_sock),
};

/*
 * For SOCK_RAW sockets; should be the same as inet_dgram_ops but without
 * udp_poll
 */
static const struct proto_ops inet_sockraw_ops = {
	.family = PF_INET,
	.release = inet_release,
	.bind = inet_bind,
	.connect = inet_dgram_connect,
	.sendmsg = inet_sendmsg,
	.recvmsg = sock_common_recvmsg,
};
