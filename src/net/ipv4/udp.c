/**
 * @file
 * @brief The User Datagram Protocol (UDP).
 *
 * @date 26.03.2009
 * @author Nikolay Korotky
 */
#include <string.h>
#include <common.h>
#include <net/inetdevice.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/socket.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <net/inet_common.h>

static struct udp_sock *udp_hash[CONFIG_MAX_KERNEL_SOCKETS];

int udp_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
//	struct inet_sock *inet = inet_sk(sk);
//	struct udp_sock *up = udp_sk(sk);
	//TODO:
	return 0;
}

int udp_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len) {
	return 0;
}

static void udp_lib_hash(struct sock *sk) {
	size_t i;
	for(i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if(udp_hash[i] == NULL) {
			udp_hash[i] = (struct udp_sock*)sk;
			break;
		}
	}
}

static void udp_lib_unhash(struct sock *sk) {
	size_t i;
	for(i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if(udp_hash[i] == (struct udp_sock*)sk) {
			udp_hash[i] = NULL;
			break;
		}
	}
}

static struct sock *udp_lookup(in_addr_t saddr, __be16 sport,
			in_addr_t daddr, __be16 dport) {
	struct inet_sock *inet;
	size_t i;
	for(i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		inet = inet_sk((struct sock*)udp_hash[i]);
		if (inet &&
			inet->rcv_saddr == daddr &&
			inet->daddr == saddr &&
			inet->dport != sport) {
			return (struct sock*)inet;
		}
	}
	return NULL;
}

static int udp_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	sock_queue_rcv_skb(sk, skb);
	return 0;
}

int udp_rcv(sk_buff_t *skb) {
	struct sock *sk;
	iphdr_t *iph = ip_hdr(skb);
	udphdr_t *uh = udp_hdr(skb);
	sk = udp_lookup(iph->saddr, uh->source, iph->daddr, uh->dest);
	if (sk) {
		udp_queue_rcv_skb(sk, skb);
	} else {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	}
	return 0;
}

#if 0
static int rebuild_udp_header(sk_buff_t *pack, unsigned short source, unsigned short dest) {
	udphdr *hdr = pack->h.uh;
	hdr->source = source;
	hdr->dest = dest;
	hdr->len = UDP_HEADER_SIZE;
	hdr->check = 0;
	hdr->check = ptclbsum(hdr, UDP_HEADER_SIZE);
	return 0;
}

static void rebuild_udp_packet(sk_buff_t *pack, struct udp_sock *sk, void *ifdev, const void *buf, int len) {
	if( pack == NULL ||
		ifdev == NULL ||
		sk ==NULL) {
		return;
	}
	pack->len = UDP_HEADER_SIZE;

	pack->h.raw = pack->data + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	memset(pack->h.raw, 0, UDP_HEADER_SIZE);
	rebuild_udp_header(pack, sk->inet.sport, sk->inet.dport);
}
#endif

int udp_disconnect(struct sock *sk, int flags) {
	return 0;
}

net_protocol_t udp_protocol = {
	.handler = udp_rcv,
	.type = IPPROTO_UDP
};

DECLARE_INET_PROTO(udp_protocol);

struct proto udp_prot = {
	.name              = "UDP",
#if 0
	.owner             = THIS_MODULE,
	.close             = udp_lib_close,
	.connect           = ip4_datagram_connect,
	.disconnect        = udp_disconnect,
	.ioctl             = udp_ioctl,
	.destroy           = udp_destroy_sock,
	.setsockopt        = udp_setsockopt,
	.getsockopt        = udp_getsockopt,
#endif
	.sendmsg           = udp_sendmsg,
	.recvmsg           = udp_recvmsg,
#if 0
	.sendpage          = udp_sendpage,
#endif
	.backlog_rcv       = udp_queue_rcv_skb,
	.hash              = udp_lib_hash,
	.unhash            = udp_lib_unhash,
#if 0
	.get_port          = udp_v4_get_port,
	.obj_size          = sizeof(struct udp_sock),
	.h.udp_table       = &udp_table,
#endif
};

const struct proto_ops inet_dgram_ops = {
	.family            = PF_INET,
#if 0
	.owner             = THIS_MODULE,
#endif
	.release           = inet_release,
	.bind              = inet_bind,
	.connect           = inet_dgram_connect,
#if 0
	.socketpair        = sock_no_socketpair,
	.accept            = sock_no_accept,
	.getname           = inet_getname,
	.poll              = udp_poll,
	.ioctl             = inet_ioctl,
	.listen            = sock_no_listen,
	.shutdown          = inet_shutdown,
	.setsockopt        = sock_common_setsockopt,
	.getsockopt        = sock_common_getsockopt,
#endif
	.sendmsg           = inet_sendmsg,
	.recvmsg           = sock_common_recvmsg,
#if 0
	.mmap              = sock_no_mmap,
	.sendpage          = inet_sendpage,
#endif
};

static struct inet_protosw udp_socket = {
	.type = SOCK_DGRAM,
	.protocol = IPPROTO_UDP,
	.prot = &udp_prot,
	.ops = &inet_dgram_ops,
	.no_check = 0 /*UDP_CSUM_DEFAULT*/
};

DECLARE_INET_SOCK(udp_socket);
