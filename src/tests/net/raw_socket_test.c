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

#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/l3/route.h>

EMBOX_TEST_SUITE("raw socket test");

TEST_SETUP_SUITE(suite_setup);
TEST_TEARDOWN_SUITE(suite_teardown);

TEST_TEARDOWN(case_teardown);

#define PORT       1

static int b, c;
static struct sockaddr_in addr;
static socklen_t addrlen;
static char buf[64];

static int create_recv_socket(int proto);

static inline struct sockaddr * to_sa(struct sockaddr_in *sa_in) {
	return (struct sockaddr *) sa_in;
}

TEST_CASE("raw socket with IPPROTO_RAW can send and receive") {
	char packet[sizeof(struct iphdr) + 1];
	/* point the iphdr to the beginning of the packet */
	struct iphdr *ip = (struct iphdr *) packet;
	int hdrincl = 1;
	size_t pkt_size = sizeof(packet);

	create_recv_socket(IPPROTO_RAW);

	c = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	test_assert(c >= 0);

	test_assert_zero(
			setsockopt(c, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl)));

	memset(packet, 'a', pkt_size);

	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = htons(pkt_size);
	ip->frag_off = 0;
	ip->ttl = 64; /* default value */
	ip->proto = IPPROTO_RAW; /* protocol at L4 */
	ip->check = 0; /* not needed in iphdr */
	ip->saddr = addr.sin_addr.s_addr;
	ip->daddr = addr.sin_addr.s_addr;

	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_equal(pkt_size, send(c, packet, pkt_size, 0));

	test_assert_equal(pkt_size, recv(b, buf, pkt_size, 0));
	test_assert_equal('a', buf[pkt_size - 1]);
}

static int create_recv_socket(int proto) {
	b = socket(AF_INET, SOCK_RAW, proto);
	if (b == -1) {
		return -errno;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(PORT);
	addrlen = sizeof addr;

	if (-1 == bind(b, to_sa(&addr), addrlen)) {
		return -errno;
	}

	if (-1 == fcntl(b, F_SETFD, O_NONBLOCK)) {
		return -errno;
	}

	return 0;
}

static int suite_setup(void) {
	int ret;
	struct in_device *in_dev;

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
	struct in_device *in_dev;

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
	if (-1 == close(b)) {
		return -errno;
	}

	if (-1 == close(c)) {
		return -errno;
	}

	return 0;
}
