/**
 * @file
 * @brief
 *
 * @date 22.02.15
 * @author Alexander Kalmuk
 */

#include <arpa/inet.h>
#include <embox/test.h>
#include <errno.h>
#include <fcntl.h>
#include <framework/mod/options.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netpacket/packet.h>

#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/l3/route.h>
#include <net/l2/ethernet.h>

EMBOX_TEST_SUITE("raw socket test");

TEST_SETUP_SUITE(suite_setup);
TEST_TEARDOWN_SUITE(suite_teardown);

TEST_TEARDOWN(case_teardown);

static int r, s;
static struct in_device *in_dev;
static char buf[128];

static int create_recv_packet_socket(int proto);
static int create_send_inet_socket(int proto);

TEST_CASE("Send from raw socket with IPPROTO_RAW protocol"
		"and receive with raw socket with htons(ETH_P_ALL) protocol") {
	char packet[sizeof(struct iphdr) + 1];
	/* point the iphdr to the beginning of the packet */
	struct iphdr *ip = (struct iphdr *) packet;
	/* size of packet piece starting from IP header */
	size_t pkt_size = sizeof(packet);
	/* size of all packet including device layer header */
	size_t all_pkt_size = pkt_size + in_dev->dev->hdr_len;

	test_assert_zero(create_send_inet_socket(IPPROTO_RAW));
	test_assert_zero(create_recv_packet_socket(htons(ETH_P_ALL)));

	memset(packet, 'a', pkt_size);

	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = htons(pkt_size);
	ip->frag_off = 0;
	ip->ttl = 64; /* default value */
	ip->proto = IPPROTO_RAW; /* protocol at L4 */
	ip->check = 0; /* not needed in iphdr */
	ip->saddr = htonl(INADDR_LOOPBACK);
	ip->daddr = htonl(INADDR_LOOPBACK);

	test_assert_equal(pkt_size, send(s, packet, pkt_size, 0));

	test_assert_equal(all_pkt_size, recv(r, buf, all_pkt_size, 0));
	test_assert_equal('a', buf[all_pkt_size - 1]);
}

static int create_recv_packet_socket(int proto) {
	struct sockaddr_ll sll;

	r = socket(PF_PACKET, SOCK_RAW, proto);
	if (r == -1) {
		return -errno;
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = in_dev->dev->index;
	sll.sll_protocol = htons(ETH_P_ALL);

	if (-1 == bind(r, (struct sockaddr *) &sll, sizeof(sll))) {
		return -errno;
	}

	if (-1 == fcntl(r, F_SETFD, O_NONBLOCK)) {
		return -errno;
	}

	return 0;
}

static int create_send_inet_socket(int proto) {
	int hdrincl = 1;
	static struct sockaddr_in addr;

	s = socket(AF_INET, SOCK_RAW, proto);
	if (s == -1) {
		return -errno;
	}

	if (-1 == setsockopt(s, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl))) {
		return -errno;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = 0; /* not needed in SOCK_RAW */

	if (-1 == connect(s, (struct sockaddr *) &addr, sizeof addr)) {
		return -errno;
	}

	return 0;
}

static int suite_setup(void) {
	int ret;

	/* setup lo device */
	in_dev = inetdev_get_loopback_dev();
	if (in_dev == NULL) {
		return -ENODEV;
	}

	ret = inetdev_set_addr(in_dev, htonl(INADDR_LOOPBACK));

	if (ret != 0) {
		return ret;
	}

	ret = netdev_flag_up(in_dev->dev, IFF_UP);
	if (ret != 0) {
		return ret;
	}

	/* add route for lo */
	ret = rt_add_route(in_dev->dev, ntohl(INADDR_LOOPBACK & ~1),
			htonl(0xFF000000), 0, RTF_UP);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static int suite_teardown(void) {
	int ret;

	/* down lo device */
	in_dev = inetdev_get_loopback_dev();
	if (in_dev == NULL) {
		return -ENODEV;
	}

	ret = netdev_flag_down(in_dev->dev, IFF_UP);
	if (ret != 0) {
		return ret;
	}

	/* del route for lo */
	ret = rt_del_route(in_dev->dev, ntohl(INADDR_LOOPBACK & ~1),
			htonl(0xFF000000), 0);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static int case_teardown(void) {
	if (-1 == close(r)) {
		return -errno;
	}

	if (-1 == close(s)) {
		return -errno;
	}

	return 0;
}
