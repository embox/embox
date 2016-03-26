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

#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <kernel/task/resource/idesc_table.h>

EMBOX_TEST_SUITE("inet dgram socket test");

TEST_SETUP_SUITE(suite_setup);
TEST_TEARDOWN_SUITE(suite_teardown);

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define PROTO OPTION_GET(NUMBER, proto)

#define PORT       1
#define OTHER_PORT 2
#define BAD_PORT   3

static int b, c;
static struct sockaddr_in addr;
static socklen_t addrlen;
static char buf[3];

static inline struct sockaddr * to_sa(struct sockaddr_in *sa_in) {
	return (struct sockaddr *)sa_in;
}

TEST_CASE("connect() works fine on right arguments") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
}

TEST_CASE("connect() may be used several times in a row") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));

	addr.sin_port = htons(OTHER_PORT);
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
}

TEST_CASE("listen() doesn't support datagram socket") {
	test_assert_equal(-1, listen(c, 0));
	test_assert_equal(EOPNOTSUPP, errno);
}

TEST_CASE("accept() doesn't support datagram socket") {
	test_assert_equal(-1, accept(c, to_sa(&addr), &addrlen));
	test_assert_equal(EOPNOTSUPP, errno);
}

TEST_CASE("send() doesn't work on disconnected socket") {
	test_assert_equal(-1, send(c, "", 1, 0));
	test_assert_equal(EDESTADDRREQ, errno);
}

TEST_CASE("send() works on connected socket") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_equal(1, send(c, "a", 1, 0));

	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('a', buf[0]);
}

TEST_CASE("sendto() and sendmsg() works on disconnected socket") {
	test_assert_equal(1, sendto(c, "a", 1, 0, to_sa(&addr),
				addrlen));

	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('a', buf[0]);

	/* TODO add test for sendmsg */
}

TEST_CASE("sendto() and sendmsg() with not-null address pointer works"
		" on connected socket") {
	addr.sin_port = htons(OTHER_PORT);
	test_assert_zero(connect(c, to_sa(&addr), addrlen));

	addr.sin_port = htons(PORT);
	test_assert_equal(1, sendto(c, "a", 1, 0, to_sa(&addr),
				addrlen));

	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('a', buf[0]);

	/* TODO add test for sendmsg */
}

TEST_CASE("recv() works on disconnected socket") {
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_equal(1, send(c, "a", 1, 0));
	test_assert_equal(1, send(c, "b", 1, 0));

	test_assert_equal(0, recv(b, buf, 0, 0));
	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('b', buf[0]);
}

TEST_CASE("recv() works on connected socket") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_zero(getsockname(c, to_sa(&tmp), &addrlen));
	test_assert_zero(connect(b, to_sa(&tmp), addrlen));

	addr.sin_port = htons(BAD_PORT);
	test_assert_equal(1, send(c, "a", 1, 0));
	test_assert_equal(1, sendto(c, "b", 1, 0, to_sa(&addr),
				addrlen));
	test_assert_equal(1, sendto(c, "c", 1, 0, NULL, 0));

	test_assert_equal(0, recv(b, buf, 0, 0));
	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('c', buf[0]);
}

#if 0
TEST_CASE("recvfrom() and recvmsg() works on disconnected"
		" socket") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_zero(getsockname(c, to_sa(&addr), &addrlen));

	test_assert_equal(1, send(c, "a", 1, 0));
	test_assert_equal(1, send(c, "b", 1, 0));

	test_assert_equal(0, recvfrom(b, buf, 0, 0, to_sa(&tmp),
				&addrlen));
	test_assert_equal(sizeof addr, addrlen);
	test_assert_mem_equal(&addr, &tmp, addrlen);
	test_assert_equal(1, recvfrom(b, buf, 2, 0, NULL, NULL));
	test_assert_equal('b', buf[0]);

	/* TODO add test for recvmsg */
}
#endif

TEST_CASE("recvfrom() and recvmsg() works on connected socket") {
	struct sockaddr_in tmp;
	test_assert_zero(connect(c, to_sa(&addr), addrlen));
	test_assert_zero(getsockname(c, to_sa(&tmp), &addrlen));
	test_assert_zero(connect(b, to_sa(&tmp), addrlen));

	addr.sin_port = htons(BAD_PORT);
	test_assert_equal(1, send(c, "a", 1, 0));
	test_assert_equal(1, sendto(c, "b", 1, 0, to_sa(&addr),
				addrlen));
	test_assert_equal(1, sendto(c, "c", 1, 0, NULL, 0));

	test_assert_equal(0, recvfrom(b, buf, 0, 0, to_sa(&addr),
				&addrlen));
	test_assert_equal(sizeof addr, addrlen);
	test_assert_mem_equal(&addr, &tmp, addrlen);
	test_assert_equal(1, recv(b, buf, 2, 0));
	test_assert_equal('c', buf[0]);

	/* TODO add test for recvmsg */
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

TEST_CASE("writev/readv")
{
	ssize_t ret;

	char *str[11];
	char *str0 = "sample ";
	char *str1 = "text";
	strcpy(str, str0);
	strcat(str, str1);

	struct iovec iov_w[2];
	struct iovec iov_r[2];

	iov_w[0].iov_base = str0;
	iov_w[0].iov_len = strlen(str0);
	iov_w[1].iov_base = str1;
	iov_w[1].iov_len = strlen(str1);

	ret = writev(c, iov_w, 2);
	read(c, str, 11);

	test_assert_equal(ret, 2);
	test_assert_mem_equal(iov_w[0].iov_base, str, iov_w[0].iov_len);
	test_assert_mem_equal(iov_w[1].iov_base, str + iov_w[0].iov_len, iov_w[1].iov_len);

	write(c, str, 11);
	ret = readv(c, iov_r, 2);

	test_assert_equal(ret, 2);
	test_assert_mem_equal(iov_r[0].iov_base, str, iov_w[0].iov_len);
	test_assert_mem_equal(iov_r[1].iov_base, str + iov_w[0].iov_len, iov_r[1].iov_len);
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
	b = socket(AF_INET, SOCK_DGRAM, PROTO);
	if (b == -1) {
		return -errno;
	}

	c = socket(AF_INET, SOCK_DGRAM, PROTO);
	if (c == -1) {
		return -errno;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	addrlen = sizeof addr;

	if (-1 == bind(b, to_sa(&addr), addrlen)) {
		return -errno;
	}

	if (-1 == fcntl(b, F_SETFD, O_NONBLOCK)) {
		return -errno;
	}

	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

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
