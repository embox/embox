/**
 * @file
 *
 * @date Oct 7, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_NET_SOCKET_NETLINK_H_
#define SRC_NET_SOCKET_NETLINK_H_

#include <net/if.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

struct net_device;

struct netlink_msg {
	struct nlmsghdr nlmsghdr;

	struct rtattr attr_if_name;
	char if_name[(IFNAMSIZ + (4 - 1)) & ~(4 - 1)];

	struct rtattr attr_if_addr;
	char if_addr[8];

	struct rtattr attr_if_link;
	int if_link;
};

extern int netlink_notify_newlink(struct net_device *net_dev);
extern int netlink_notify_dellink(struct net_device *net_dev);

#endif /* SRC_NET_SOCKET_NETLINK_H_ */
