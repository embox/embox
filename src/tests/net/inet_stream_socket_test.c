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
#include <fcntl.h>
#include <framework/mod/options.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/l3/route.h>

EMBOX_TEST_SUITE("inet stream socket test");

TEST_SETUP_SUITE(suite_setup);
TEST_TEARDOWN_SUITE(suite_teardown);

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define PROTO OPTION_GET(NUMBER, proto)

#define PORT     1
#define BAD_PORT 2

static int l, c, a;
static struct sockaddr_in addr;
static socklen_t addrlen;
static char buf[3];

static inline struct sockaddr * to_sa(struct sockaddr_in *sa_in) {
	return (struct sockaddr *)sa_in;
}

TEST_CASE("connect() fails on listening sockets") {
	test_assert_equal(-1, connect(l, to_sa(&addr), addrlen));
	test_assert_equal(EISCONN, errno);
}

TEST_CASE("connect() fails on non-avaiable addresses") {
	addr.sin_port = htons(BAD_PORT);
	test_assert_equal(-1, connect(c, to_sa(&addr), addrlen));
	test_assert_equal(ECONNRESET, errno);
}

TEST_CASE("connect() works fine on right arguments") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
}

TEST_CASE("listen() may be used several times in a row") {
	test_assert_zero(listen(l, -1));
	test_assert_zero(listen(l, 0));
	test_assert_zero(listen(l, 2));
}

TEST_CASE("accept() fails on non-listening sockets") {
	test_assert_equal(-1, accept(c, to_sa(&addr), &addrlen));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("accept() can accept incoming connections") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	a = accept(l, to_sa(&addr), &addrlen);
	test_assert(0 <= a);
	test_assert_equal(sizeof addr, addrlen);

	test_assert_zero(close(a));
}

TEST_CASE("send() doesn't work on disconnected socket") {
	test_assert_equal(-1, send(c, "", 1, 0));
	test_assert_equal(ENOTCONN, errno);
}

TEST_CASE("send() fails on listening socket") {
	test_assert_equal(-1, send(l, "", 1, 0));
	test_assert_equal(EPIPE, errno);
}

TEST_CASE("sendto() and sendmsg() fails on not-null address"
		" pointer") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));

	test_assert_equal(-1, sendto(c, "", 1, 0, to_sa(&addr),
				addrlen));
	test_assert_equal(EISCONN, errno);

	/* TODO add test for sendmsg */
}

TEST_CASE("recv(), recvfrom() and recvmsg() fails on listening"
		" socket") {
	test_assert_equal(-1, recv(l, buf, 1, 0));
	test_assert_equal(ENOTCONN, errno);

	test_assert_equal(-1, recvfrom(l, buf, 1, 0, to_sa(&addr),
				&addrlen));
	test_assert_equal(ENOTCONN, errno);

	/* TODO add test for recvmsg */
}

TEST_CASE("send...() and recv...() works") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	a = accept(l, to_sa(&addr), &addrlen);
	test_assert(0 <= a);
	test_assert_zero(fcntl(a, F_SETFD, O_NONBLOCK));

	test_assert_equal(0, send(c, NULL, 0, 0));
	test_assert_equal(1, send(c, "a", 1, 0));

	test_assert_equal(0, recv(a, buf, 0, 0));
	test_assert_equal(1, recv(a, buf, 2, 0));
	test_assert_equal('a', buf[0]);

	test_assert_equal(0, sendto(c, NULL, 0, 0, NULL, 0));
	test_assert_equal(1, sendto(c, "b", 1, 0, NULL, 0));

	test_assert_equal(0, recvfrom(a, NULL, 0, 0, to_sa(&tmp),
				&addrlen));
	test_assert_equal(sizeof tmp, addrlen);
	test_assert_mem_equal(&addr, &tmp, addrlen);
	test_assert_equal(1, recvfrom(a, buf, 2, 0, NULL, NULL));
	test_assert_equal('b', buf[0]);

	/* TODO add test for sendmsg/recvmsg */

	test_assert_zero(close(a));
}

TEST_CASE("accept() returns first connected client") {
	int other_c = socket(AF_INET, SOCK_STREAM, PROTO);
	test_assert(other_c >= 0);
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_zero(connect(other_c, to_sa(&addr), addrlen));
	a = accept(l, to_sa(&addr), &addrlen);
	test_assert(0 <= a);
	test_assert_zero(fcntl(a, F_SETFD, O_NONBLOCK));

	test_assert_equal(1, send(c, "a", 1, 0));
	test_assert_equal(1, recv(a, buf, 2, 0));
	test_assert_equal('a', buf[0]);

	close(other_c);
}

TEST_CASE("getsockname() returns not unspecified address when"
		" a socket was connected") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));

	test_assert_zero(getsockname(c, to_sa(&tmp), &addrlen));
	test_assert_equal(sizeof addr, addrlen);
	addr.sin_port = tmp.sin_port;
	test_assert_mem_equal(&addr, &tmp, addrlen);
}

TEST_CASE("getpeername() returns address of peer") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));

	test_assert_zero(getpeername(c, to_sa(&tmp), &addrlen));
	test_assert_equal(sizeof addr, addrlen);
	test_assert_mem_equal(&addr, &tmp, addrlen);
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

static int case_setup(void) {
	l = socket(AF_INET, SOCK_STREAM, PROTO);
	if (l == -1) {
		return -errno;
	}

	if (-1 == listen(l, 3)) {
		return -errno;
	}

	c = socket(AF_INET, SOCK_STREAM, PROTO);
	if (c == -1) {
		return -errno;
	}

	addrlen = sizeof addr;
	if (-1 == getsockname(l, to_sa(&addr), &addrlen)) {
		return -errno;
	}

	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	return 0;
}

static int case_teardown(void) {
	if (-1 == close(l)) {
		return -errno;
	}

	if (-1 == close(c)) {
		return -errno;
	}

	return 0;
}
