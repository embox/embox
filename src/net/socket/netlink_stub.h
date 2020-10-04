/**
 * @file
 *
 * @date Oct 7, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_NET_SOCKET_NETLINK_H_
#define SRC_NET_SOCKET_NETLINK_H_

struct netlink_msg {
	char stub[16];
};

struct net_device;

static inline int netlink_notify_newlink(struct net_device *net_dev) {
	(void)net_dev;
	return 0;
}

static inline int netlink_notify_dellink(struct net_device *net_dev) {
	(void)net_dev;
	return 0;
}

#endif /* SRC_NET_SOCKET_NETLINK_H_ */
