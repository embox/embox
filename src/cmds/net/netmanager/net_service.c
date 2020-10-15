/**
 * @file
 *
 * @date Oct 4, 2020
 * @autor Anton Bondarev
 */


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <net/netlink.h>

#include <net/inetdevice.h>
#include <net/netdevice.h>

static void parse_rtattr(struct rtattr *rta, int len, char **if_name) {
	while (RTA_OK(rta, len)) {

		switch(rta->rta_type) {
		case IFLA_IFNAME:
			*if_name = RTA_DATA(rta);
			break;
#if 0
		case IFLA_ADDRESS:
			*if_addr = RTA_DATA(rta);
			break;
#endif
		}
		rta = RTA_NEXT(rta, len);
	}
}

static void iface_up(char dev_name[IFNAMSIZ]) {
	char system_str[0x12];

	memset(system_str, 0, sizeof(system_str));
	strcpy(system_str, "netmanager ");
	strcat(system_str, dev_name);

	system(system_str);
}

static void iface_down(char dev_name[IFNAMSIZ]) {
	char system_str[0x20];

	memset(system_str, 0, sizeof(system_str));
	strcpy(system_str, "ifconfig ");
	strcat(system_str, dev_name);
	strcat(system_str, " down");

	system(system_str);
}

int main(int argc, char **argv) {
	int fd;
	struct in_device *iface;
	struct sockaddr_nl local;
	struct msghdr msg;
	char buf[sizeof(struct netlink_msg)];
	struct iovec iov;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

	if (fd < 0) {
		printf("Failed to create netlink socket: %s\n",
				(char*) strerror(errno));
		return 1;
	}

	memset(&local, 0, sizeof(local));

	local.nl_family = AF_NETLINK;
	local.nl_groups = RTMGRP_LINK;
	local.nl_pid = getpid();

	if (bind(fd, (struct sockaddr*) &local, sizeof(local)) < 0) {
		printf("Failed to bind netlink socket: %s\n", (char*) strerror(errno));
		close(fd);
		return 1;
	}

	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);

	msg.msg_name = &local;
	msg.msg_namelen = sizeof(local);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	for (iface = inetdev_get_first(); iface != NULL;
			iface = inetdev_get_next(iface)) {
		iface_up(iface->dev->name);
	}

	while (1) {
		ssize_t len;
		struct nlmsghdr *nh;

		len = recvmsg(fd, &msg, 0);

		if (len == 0) {
			continue;
		}

		if (msg.msg_namelen != sizeof(local)) {
			printf("Invalid length of the sender address struct\n");
			continue;
		}

		for (nh = (struct nlmsghdr*) buf; NLMSG_OK(nh, len);
				nh = NLMSG_NEXT(nh, len)) {
			char *if_addr;
			char *if_name;

			/* The end of multipart message */
			if (nh->nlmsg_type == NLMSG_DONE) {
				break;
			}

			if (nh->nlmsg_type == NLMSG_ERROR) {
				/* Do some error handling */
			}

			if_addr = NULL;

			parse_rtattr(NLMSG_DATA(nh), nh->nlmsg_len, &if_name);

			switch (nh->nlmsg_type) {
			case RTM_DELADDR:
				printf("Interface %s: address was removed\n", if_name);
				break;

			case RTM_DELLINK:
				printf("Network interface %s was removed\n", if_name);
				iface_down(if_name);
				break;

			case RTM_NEWLINK:
				printf("New network interface %s\n", if_name);
				iface_up(if_name);
				break;

			case RTM_NEWADDR:
				printf("Interface %s: new address was assigned: %s\n", if_name,
						if_addr);
				break;
			}
		}

		len -= NLMSG_ALIGN(len);

		nh = (struct nlmsghdr*) ((char*) nh + NLMSG_ALIGN(len));
	}

	close(fd); // close socket

	return 0;
}
