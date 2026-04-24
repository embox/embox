/**
 * @file
 *
 * @brief PF_UNIX protocol family socket handler
 *
 * @date 31.01.2012
 * @author Anton Bondarev
 */

 #include <errno.h>
#include <assert.h>
#include <sys/socket.h>

#include <net/family.h>
#include <net/net_sock.h>
#include <net/sock.h>
#include <netinet/in.h>

#include <linux/can.h>

#include <embox/net/pack.h>
#include <net/netdevice.h>
#include <net/can_netdevice.h>
#include <net/skbuff.h>

#include <util/math.h>

#include <mem/misc/pool.h>

#define MODOPS_AMOUNT_CAN_SOCK   8

/* Prototypes */
static const struct sock_family_ops af_can_raw_ops;

static const struct net_family_type af_can_types[] = {
	{ SOCK_DGRAM, &af_can_raw_ops },
	{ SOCK_RAW, &af_can_raw_ops }
};

static const struct net_pack_out_ops can_out_ops_struct;
/**
 * IPV6 packet outgoing options
 */
const struct net_pack_out_ops *const af_can_ops = &can_out_ops_struct;

EMBOX_NET_FAMILY(AF_CAN, af_can_types, af_can_ops);

struct af_can_sock {
	/* sk has to be the first member */
	struct sock sk;
	struct sockaddr_can skaddr_can;
	int can_ifindex;
};

static int af_can_init(struct sock *sk) {

	assert(sk);

	return 0;
}

static int af_can_close(struct sock *sk) {
	assert(sk);

	assert(sk->p_ops != NULL);

	if (sk->p_ops->close == NULL) {
		sock_release(sk);
		return 0;
	}

	return sk->p_ops->close(sk);

}

static int af_can_shutdown(struct sock *sk, int how) {
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->shutdown == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->shutdown(sk, how);
}


static int af_can_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	struct af_can_sock *skcan;

	assert(sk);

	skcan = (struct af_can_sock *)sk;
	memcpy(&skcan->skaddr_can, addr, min(sizeof(skcan->skaddr_can), addrlen));

	sk->sock_netdev = cannetdev_get(NULL, skcan->skaddr_can.can_ifindex);
	if (NULL == sk->sock_netdev) {
		return -ENOTSUP;
	}

	return 0;
}

static int af_can_bind_local(struct sock *sk) {
	return 0;
}

static struct sk_buff *af_can_make_skb(struct msghdr *msg) {
	struct sk_buff *skb;

	skb = skb_alloc(msg->msg_iov->iov_len);
	if (skb == NULL) {
		return NULL;
	}

	memcpy(skb->mac.raw , msg->msg_iov->iov_base, msg->msg_iov->iov_len);

	return skb;
}

static int af_can_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct sk_buff *skb;

	skb = af_can_make_skb(msg);

	if (skb == NULL) {
		return -ENOMEM;
	}

	sk->sock_netdev->drv_ops->xmit(sk->sock_netdev, skb);

	return 0;
}

static int af_can_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	return 0;
}


POOL_DEF(af_can_sock_pool, struct af_can_sock, MODOPS_AMOUNT_CAN_SOCK);

static const struct sock_family_ops af_can_raw_ops = {
	.init        = af_can_init,
	.close       = af_can_close,
	.bind        = af_can_bind,
	.bind_local  = af_can_bind_local,
	.sendmsg     = af_can_sendmsg,
	.recvmsg     = af_can_recvmsg,
	.shutdown    = af_can_shutdown,
	.sock_pool   = &af_can_sock_pool
};