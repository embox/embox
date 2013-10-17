/**
 * @file
 * @brief
 *
 * @date 05.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <embox/test.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/netdevice.h>

EMBOX_TEST_SUITE("inet socket test");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define TYPE  OPTION_GET(NUMBER, type)
#define PROTO OPTION_GET(NUMBER, proto)

#define PORT       1
#define OTHER_PORT 2
#define BAD_ADDR   3

static int sock;
static struct sockaddr_in addr;
static socklen_t addrlen;
static struct msghdr msg;

static inline struct sockaddr * to_sa(struct sockaddr_in *sa_in) {
	return (struct sockaddr *)sa_in;
}

TEST_CASE("bind() fails on non-local address") {
	addr.sin_addr.s_addr = htonl(BAD_ADDR);
	test_assert_equal(-1, bind(sock, to_sa(&addr), addrlen));
	test_assert_equal(EADDRNOTAVAIL, errno);
}

TEST_CASE("bind() works for local address") {
	test_assert_zero(bind(sock, to_sa(&addr), addrlen));
}

TEST_CASE("bind() fails in case of busy address") {
	int other_sock = socket(AF_INET, TYPE, PROTO);
	test_assert(other_sock >= 0);
	test_assert_zero(bind(other_sock, to_sa(&addr), addrlen));

	test_assert_equal(-1, bind(sock, to_sa(&addr), addrlen));
	test_assert_equal(EADDRINUSE, errno);

	test_assert_zero(close(other_sock));
}

TEST_CASE("bind() fails for bound socket") {
	test_assert_zero(bind(sock, to_sa(&addr), addrlen));

	addr.sin_port = htons(OTHER_PORT);
	test_assert_equal(-1, bind(sock, to_sa(&addr), addrlen));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("connect() fails on unreachable address") {
	addr.sin_addr.s_addr = htonl(BAD_ADDR);
	test_assert_equal(-1, connect(sock, to_sa(&addr), addrlen));
	test_assert_equal(ENETUNREACH, errno);
}

TEST_CASE("getsockname() returns unspecified address when a socket"
		" was created") {
	struct sockaddr_in tmp;
	addr.sin_port = 0;

	test_assert_zero(getsockname(sock, to_sa(&tmp), &addrlen));
	test_assert_equal(sizeof addr, addrlen);
	test_assert_mem_equal(&addr, &tmp, addrlen);
}

static int case_setup(void) {
	static struct iovec iov;
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

	/* init local */
	sock = socket(AF_INET, TYPE, PROTO);
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	addrlen = sizeof addr;
	memset(&msg, 0, sizeof msg);
	memset(&iov, 0, sizeof iov);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	return sock >= 0 ? 0 : -errno;
}

static int case_teardown(void) {
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
	ret = rt_del_route(inetdev_get_loopback_dev()->dev,
			ntohl(INADDR_LOOPBACK & ~1), htonl(0xFF000000), 0);
	if (ret != 0) {
		return ret;
	}

	/* fini local */
	ret = close(sock);
	sock = -1;
	return 0 == ret ? 0 : -errno;
}
