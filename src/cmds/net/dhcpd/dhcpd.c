/**
 * @file
 *
 * @date May 26, 2020
 * @author Anton Bondarev
 */
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include <net/lib/bootp.h>
#include <net/inetdevice.h>
#include <net/l2/ethernet.h>
#include <netpacket/packet.h>
#include <net/l3/arp.h>
#include <net/neighbour.h>

#include <framework/mod/options.h>

#define BIND_TO_DEV        OPTION_GET(NUMBER, bind_to_dev)
#define DHCPD_IF_NAME      OPTION_STRING_GET(if_name)
#define NEIGHBOUR_IP_ADDR  OPTION_STRING_GET(client_ip_addr)

static int dhcpd_create_socket(struct net_device *dev) {
	int sock;
	struct sockaddr_in our;
	int ret;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock  < 0) {
		return sock;
	}

	memset(&our, 0, sizeof(our));

	our.sin_family = AF_INET;
	our.sin_port = htons(BOOTP_SERVER_PORT);
	our.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sock, (void *) &our, sizeof(our));
	if (ret  < 0) {
		ret = -errno;
		goto error;
	}
#if BIND_TO_DEV
	if (-1 == setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,
				&dev->name[0], strlen(&dev->name[0]))) {
		ret = -errno;
		goto error;
	}
#endif
	return sock;

error:
	close(sock);

	return ret;
}


static int dhcpd_parse_discover(struct net_device *dev, int sock, struct bootphdr *bootphdr) {
	in_addr_t ip_addr;
	struct sockaddr_in addr;
	struct bootphdr out_bootphdr;
	struct in_device *in_dev;
	int ret = 0;
	uint8_t msg_type;
	struct dhcp_option opt;

	in_dev = inetdev_get_by_dev(dev);
	ip_addr = inet_addr(NEIGHBOUR_IP_ADDR);

	neighbour_add(ETH_P_IP, &ip_addr, sizeof(in_addr_t), dev,
			ARP_HRD_ETHERNET, bootphdr->chaddr, bootphdr->hlen, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(BOOTP_CLIENT_PORT);
	addr.sin_addr.s_addr = ip_addr;

	opt.tag = TAG_DHCP_MESS_TYPE;

	if (0 == bootp_find_option(bootphdr, &opt) ) {
		return 0;
	}

	switch(opt.value[0]) {
	case DHCP_MSG_TYPE_DISCOVER:
		msg_type = DHCP_MSG_TYPE_OFFER;
		break;
	case DHCP_MSG_TYPE_REQUEST:
		msg_type = DHCP_MSG_TYPE_ACK;
		break;
	default:
		return 0;
	}

	bootp_build_offer(&out_bootphdr, bootphdr->xid, msg_type,
			dev->type, bootphdr->hlen, bootphdr->chaddr,
			ip_addr, inetdev_get_addr(in_dev));

	sendto(sock, &out_bootphdr, sizeof(out_bootphdr), 0,
			(const struct sockaddr *)&addr, sizeof addr);

	return ret;
}

int main(int argc, char **argv) {
	struct net_device *dev;
	int sock;
	struct bootphdr bootphdr;
	struct sockaddr_in addr;
	int ret;
	socklen_t sklen = sizeof(struct sockaddr_in);

	dev = netdev_get_by_name(DHCPD_IF_NAME);

	sock = dhcpd_create_socket(dev);
	if (sock < 0) {
		return sock;
	}

	while(1) {

		ret = recvfrom(sock, &bootphdr, sizeof(bootphdr), 0, (struct sockaddr *)&addr, &sklen);
		if ( ret < 0) {
			close(sock);
			return ret;
		}
		switch(bootphdr.op) {
		case BOOTPREQUEST:
			dev = netdev_get_by_name(DHCPD_IF_NAME);
			dhcpd_parse_discover(dev, sock, &bootphdr);
		default:
			continue;
		}

	}

	close(sock);
	return 0;
}
