/**
 * @file
 * @brief Implements raw socket function.
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

#include <kernel/printk.h>

static struct proto raw_prot;
static const struct proto_ops inet_sockraw_ops;

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_IP, raw_prot, inet_sockraw_ops, 0, true);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, raw_prot, inet_sockraw_ops, 0, false);

static raw_sock_t *raw_hash[CONFIG_MAX_KERNEL_SOCKETS];

static int raw_rcv_skb(struct sock * sk, sk_buff_t * skb);

/* static method for getting hash table index of a socket */
static int _raw_v4_get_hash_idx(struct sock *sk){
	unsigned int i;

	if(sk != NULL){
		for(i=0; i<sizeof raw_hash / sizeof raw_hash[0]; i++)
			if(&raw_hash[i]->inet.sk == sk)
				return i;
	}
	return -1;
}

/* looking up for a socket by index in table, proto, source and dest addresses */
static struct sock *_raw_v4_lookup(unsigned int sk_hash_idx, unsigned char protocol,
									 unsigned int saddr, unsigned int daddr,
									 struct net_device *dev){
	/* socket for iterating */
	struct sock * sk_it;
	struct inet_sock *inet;
	int i;

	for(i = sk_hash_idx; i<sizeof raw_hash / sizeof raw_hash[0]; i++){
		sk_it = &raw_hash[i]->inet.sk;
		inet = inet_sk(sk_it);
		/* the socket is being searched for by (daddr, saddr, protocol) */
		if(!(inet->daddr != daddr && inet->daddr) &&
			 !(inet->rcv_saddr != saddr && inet->rcv_saddr) &&
			 /* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			 sk_it->sk_protocol == protocol){
			return sk_it;
		}
	}
	return NULL;
}


static int raw_init(struct sock *sk) {
	return ENOERR;
}

#if 0
static struct sock *raw_lookup(__u8 proto) {
	struct sock *sk;
	size_t i;
	for (i = 0; i < sizeof raw_hash / sizeof raw_hash[0]; i++) {
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
	for (i = 0; i < sizeof raw_hash / sizeof raw_hash[0]; i++) {
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
	struct sock *sk = NULL;
	int sk_idx = 0;
	struct iphdr *emb_pack_iphdr;

	emb_pack_iphdr = (struct iphdr*)(skb->h.raw+IP_HEADER_SIZE(skb->nh.iph)+ICMP_HEADER_SIZE);

	/* notify all sockets matching source, dest address and protocol */
	do{
		sk = _raw_v4_lookup(sk_idx, emb_pack_iphdr->proto,
												emb_pack_iphdr->saddr, emb_pack_iphdr->daddr, skb->dev);
		if(sk){  /* notify socket about an error */
			ip_v4_icmp_err_notify(sk, skb->h.icmph->type,
														skb->h.icmph->code);
			/* do something else - specific for raw sockets ? */
			sk_idx = _raw_v4_get_hash_idx(sk) + 1;
		}
	}while(sk != NULL);
}

static void raw_close(struct sock *sk, long timeout) {
	sk_common_release(sk);
}

static int raw_rcv_skb(struct sock *sk, sk_buff_t *skb) {
	if (sock_queue_rcv_skb(sk, skb) < 0) {
		return NET_RX_DROP;
	}
	return NET_RX_SUCCESS;
}

static void raw_v4_hash(struct sock *sk) {
	size_t i;
	for (i = 0; i < sizeof raw_hash / sizeof raw_hash[0]; i++) {
		if (raw_hash[i] == NULL) {
			raw_hash[i] = (raw_sock_t *) sk;
			break;
		}
	}
}

static void raw_v4_unhash(struct sock *sk) {
	size_t i;
	for (i = 0; i < sizeof raw_hash / sizeof raw_hash[0]; i++) {
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
	assert(skb);
	memcpy((void*)((unsigned int)(skb->mac.raw + ETH_HEADER_SIZE)),
					(void*) msg->msg_iov->iov_base, len);
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

		/* Correct until somebody sends:
		 *	IP packet with options
		 *	already fragmented IP packet
		 * Probably we don't need this pointer in later code
		 */
	skb->h.raw = skb->mac.raw + ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;

	skb->sk = sk;
	return ip_send_packet(inet, skb);
}

static int raw_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags) {
	struct sk_buff *skb;

	skb = skb_recv_datagram(sk, flags, 0, 0);
	if (skb && (skb->len > 0)) {
		if (len > (skb->len - ETH_HEADER_SIZE)) {
			len = skb->len - ETH_HEADER_SIZE;
		}
		memcpy((void*) msg->msg_iov->iov_base,
				(void*) (skb->mac.raw + ETH_HEADER_SIZE), len);
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
	return ENOERR;
}

static int raw_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return ENOERR;
}

static int raw_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return ENOERR;
}

static int raw_ioctl(struct sock *sk, int cmd, unsigned long arg) {
	return ENOERR;
}

int ip4_datagram_connect(struct sock *sk,
				struct sockaddr *uaddr, int addr_len) {
	return ENOERR;
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
	.compare_addresses = inet_address_compare,
};
