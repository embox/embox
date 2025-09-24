/**
 * @file
 *
 * @brief AF_NETLINK protocol family socket handler
 *
 * @date 31.01.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/socket.h>

#include <util/math.h>

#include <mem/misc/pool.h>

#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <net/if.h>

#include <net/sock.h>
#include <net/netlink.h>
#include <net/inetdevice.h>

#include <net/family.h>
#include <net/net_sock.h>

#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <framework/mod/options.h>

#define MAX_SOCKS     OPTION_GET(NUMBER,max_socks)

struct netlink_sock {
	/* sk has to be the first member */
	struct sock sk;
	struct sockaddr_nl sockaddr_nl;

	struct netlink_msg netlink_msg;
	volatile int ready_to_read;
	struct thread *thread;
};

POOL_DEF(af_netlink_sock_pool, struct netlink_sock, MAX_SOCKS);

static struct netlink_sock *netlink_socket;

/********************** netlink *******************/
static inline void nlmsghdr_init(struct nlmsghdr *hdr,
		uint32_t len, uint16_t  type, uint16_t flags, uint32_t seq, uint32_t pid) {
	hdr->nlmsg_len = len;
	hdr->nlmsg_type = type;
	hdr->nlmsg_flags = flags;
	hdr->nlmsg_seq = seq;
	hdr->nlmsg_pid = pid;
}

int netlink_notify_newlink(struct net_device *net_dev) {
	struct netlink_msg *nt_msg;

	if(!netlink_socket) {
		return 0;
	}

	nt_msg = &netlink_socket->netlink_msg;

	sched_lock();
	{
		memset(nt_msg, 0, sizeof(*nt_msg));

		nlmsghdr_init(&nt_msg->nlmsghdr,
				sizeof(struct netlink_msg),
				RTM_NEWLINK,
				0, 0, netlink_socket->sockaddr_nl.nl_pid);

		nt_msg->attr_if_name.rta_len =  sizeof(nt_msg->if_name) + 4;
		nt_msg->attr_if_name.rta_type = IFLA_IFNAME;
		memcpy(nt_msg->if_name, net_dev->name, IFNAMSIZ);
		netlink_socket->ready_to_read = 1;
		sched_wakeup(&netlink_socket->thread->schedee);
	}
	sched_unlock();

	return 0;
}

int netlink_notify_dellink(struct net_device *net_dev) {
	struct netlink_msg *nt_msg;

	if(!netlink_socket) {
		return 0;
	}

	nt_msg = &netlink_socket->netlink_msg;

	sched_lock();
	{
		memset(nt_msg, 0, sizeof(*nt_msg));

		nlmsghdr_init(&nt_msg->nlmsghdr,
				sizeof(struct netlink_msg),
				RTM_DELLINK,
				0, 0, netlink_socket->sockaddr_nl.nl_pid);

		nt_msg->attr_if_name.rta_len =  sizeof(nt_msg->if_name) + 4;
		nt_msg->attr_if_name.rta_type = IFLA_IFNAME;
		memcpy(nt_msg->if_name, net_dev->name, IFNAMSIZ);
		netlink_socket->ready_to_read = 1;
		sched_wakeup(&netlink_socket->thread->schedee);
	}
	sched_unlock();

	return 0;
}
/******* END netlink *************/

/******************af_netlink_sock ***************/
static int af_netlink_sock_init(struct sock *sock) {
	return ENOERR;
}

static int af_netlink_sock_close(struct sock *sk) {
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->close == NULL) {
		sock_release(sk);
		return 0;
	}

	return sk->p_ops->close(sk);
}

static int af_netlink_sock_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	struct netlink_sock *netlink_sock;

	netlink_sock = (struct netlink_sock *)sk;

	memcpy(&netlink_sock->sockaddr_nl, addr, sizeof(netlink_sock->sockaddr_nl));
	netlink_sock->ready_to_read = 0;
	netlink_sock->thread = thread_self();

	netlink_socket = (struct netlink_sock *)sk;

	return 0;
}

static int af_netlink_sock_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct netlink_sock *netlink_sock;
	int len;
	int ret;

	netlink_sock = (struct netlink_sock *)sk;

	sched_lock();
	{
		if (!(netlink_sock->ready_to_read)) {
			ret = SCHED_WAIT_TIMEOUT(netlink_sock->ready_to_read, 10000);
			if (ret) {
				len = 0;
				goto out;
			}
		}
		len = min(msg->msg_iov->iov_len, sizeof(netlink_sock->netlink_msg));
		memcpy(msg->msg_iov->iov_base, &netlink_sock->netlink_msg, len);
		netlink_sock->ready_to_read = 0;
	}
out:
	sched_unlock();

	return len;
}

static const struct sock_family_ops netlink_ops = {
	.init        = af_netlink_sock_init,
	.close       = af_netlink_sock_close,
	.bind        = af_netlink_sock_bind,
	.recvmsg     = af_netlink_sock_recvmsg,
	.sock_pool   = &af_netlink_sock_pool
};

static const struct net_family_type netlink_types[] = {
	{ SOCK_RAW, &netlink_ops }
};

struct net_pack_out_ops;
static const struct net_pack_out_ops *out_ops_struct;

EMBOX_NET_FAMILY(AF_NETLINK, netlink_types, out_ops_struct);

static DLIST_DEFINE(netlink_sock_list);

static const struct sock_proto_ops netlink_sock_ops = {
		.sock_list = &netlink_sock_list
};

EMBOX_NET_SOCK(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE, 1, netlink_sock_ops);
