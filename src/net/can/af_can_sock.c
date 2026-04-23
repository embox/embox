/**
 * @file
 *
 * @date 23.04.26
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <util/math.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/net_sock.h>
#include <embox/net/pack.h>

#include <linux/can.h>

#include <net/sock.h>

#include <lib/libds/dlist.h>


static const struct sock_proto_ops af_can_sock_ops_struct;
const struct sock_proto_ops *const af_can_sock_ops = &af_can_sock_ops_struct;

EMBOX_NET_SOCK(AF_CAN, SOCK_RAW, CAN_RAW, 1, af_can_sock_ops_struct);

static int af_can_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	return 0;
}

static DLIST_DEFINE(af_can_sock_list);

static int af_can_fillmsg(struct sock *sk, struct msghdr *msg,
		struct sk_buff *skb) {

	return 0;
}

static int af_can_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	return 0;
}

static const struct sock_proto_ops af_can_sock_ops_struct = {
	.sendmsg   = af_can_sendmsg,
	.recvmsg   = af_can_recvmsg,
	.fillmsg   = af_can_fillmsg,
	.sock_list = &af_can_sock_list
};
