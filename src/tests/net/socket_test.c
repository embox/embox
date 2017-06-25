/**
 * @file
 * @brief
 *
 * @date 04.10.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <sys/socket.h>
#include <unistd.h>

EMBOX_TEST_SUITE("general socket test");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define FAMILY OPTION_GET(NUMBER, family)
#define TYPE   OPTION_GET(NUMBER, type)
#define PROTO  OPTION_GET(NUMBER, proto)

#define BAD_FAMILY -1
#define BAD_TYPE   -1
#define BAD_PROTO  -1
#define BAD_SOCK   -1
#define BAD_SHUT   -1
#define BAD_LEN     0

static int sock;
static struct sockaddr addr;
static socklen_t addrlen;
static struct msghdr msg;

TEST_CASE("socket() fails on unknown family") {
	test_assert_equal(-1, socket(BAD_FAMILY, TYPE, PROTO));
	test_assert_equal(EAFNOSUPPORT, errno);
}

TEST_CASE("socket() fails on unknown socket type") {
	test_assert_equal(-1, socket(FAMILY, BAD_TYPE, PROTO));
	test_assert_equal(EPROTOTYPE, errno);
}

TEST_CASE("socket() fails on unknown protocol") {
	test_assert_equal(-1, socket(FAMILY, TYPE, BAD_PROTO));
	test_assert_equal(EPROTONOSUPPORT, errno);
}

TEST_CASE("close() fails on bad descripor") {
	test_assert_equal(-1, close(BAD_SOCK));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("socket can be created and destroyed successfully") {
	int sock = socket(FAMILY, TYPE, PROTO);
	test_assert(0 <= sock);
	test_assert_zero(close(sock));
}

TEST_CASE("close() fails on a double release") {
	int sock = socket(FAMILY, TYPE, PROTO);
	test_assert(0 <= sock);
	test_assert_zero(close(sock));

	test_assert_equal(-1, close(sock));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("bind() fails on bad descriptor") {
	test_assert_equal(-1, bind(BAD_SOCK, &addr, addrlen));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("bind() fails on invalid arguments") {
	test_assert_equal(-1, bind(sock, NULL, addrlen));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, bind(sock, &addr, BAD_LEN));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("bind() fails in case of invalid sockaddr family") {
	addr.sa_family = AF_UNSPEC;
	test_assert_equal(-1, bind(sock, &addr, addrlen));
	test_assert_equal(EAFNOSUPPORT, errno);
}

TEST_CASE("connect() fails on bad descriptor") {
	test_assert_equal(-1, connect(BAD_SOCK, &addr, addrlen));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("connect() fails on invalid arguments") {
	test_assert_equal(-1, connect(sock, NULL, addrlen));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, connect(sock, &addr, BAD_LEN));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("connect() fails in case of invalid sockaddr family") {
	addr.sa_family = AF_UNSPEC;
	test_assert_equal(-1, connect(sock, &addr, addrlen));
	test_assert_equal(EAFNOSUPPORT, errno);
}

TEST_CASE("listen() fails on bad descriptor") {
	test_assert_equal(-1, listen(BAD_SOCK, 1));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("accept() fails on bad descriptor") {
	test_assert_equal(-1, accept(BAD_SOCK, &addr, &addrlen));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("accept() fails on invalid arguments") {
	socklen_t bad_len = BAD_LEN;
	test_assert_equal(-1, accept(sock, NULL, &addrlen));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, accept(sock, &addr, NULL));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, accept(sock, &addr, &bad_len));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("send(), sendto() and sendmsg() fails on"
		" bad descriptor") {
	test_assert_equal(-1, send(BAD_SOCK, NULL, 0, 0));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, sendto(BAD_SOCK, NULL, 0, 0, NULL, 0));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, sendmsg(BAD_SOCK, &msg, 0));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("send(), sendto() and sendmsg() fails on"
		" invalid arguments") {
	test_assert_equal(-1, sendmsg(sock, NULL, 0));
	test_assert_equal(EINVAL, errno);
	msg.msg_iovlen = 0;
	test_assert_equal(-1, sendmsg(sock, &msg, 0));
	test_assert_equal(EINVAL, errno);
	msg.msg_iov = NULL;
	test_assert_equal(-1, sendmsg(sock, &msg, 0));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("recv(), recvfrom() and recvmsg() fails on"
		" bad descriptor") {
	test_assert_equal(-1, recv(BAD_SOCK, NULL, 0, 0));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, recvfrom(BAD_SOCK, NULL, 0, 0,
				NULL, NULL));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, recvmsg(BAD_SOCK, &msg, 0));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("recv(), recvfrom() and recvmsg() fails on"
		" invalid arguments") {
	test_assert_equal(-1, recvmsg(sock, NULL, 0));
	test_assert_equal(EINVAL, errno);
	msg.msg_iovlen = 0;
	test_assert_equal(-1, recvmsg(sock, &msg, 0));
	test_assert_equal(EINVAL, errno);
	msg.msg_iov = NULL;
	test_assert_equal(-1, recvmsg(sock, &msg, 0));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("shutdown() fails on bad descriptor") {
	test_assert_equal(-1, shutdown(BAD_SOCK, SHUT_RD));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, shutdown(BAD_SOCK, SHUT_WR));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, shutdown(BAD_SOCK, SHUT_RDWR));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("shutdown() fails on invalid arguments") {
	test_assert_equal(-1, shutdown(sock, BAD_SHUT));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("send(), sendto() and sendmsg() fails"
		" after shutdown with SHUT_WR") {
	test_assert_equal(-1, shutdown(sock, SHUT_WR));
	test_assert_equal(ENOTCONN, errno);

	test_assert_equal(-1, send(sock, NULL, 0, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, sendto(sock, NULL, 0, 0,
				NULL, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, sendmsg(sock, &msg, 0));
	test_assert_equal(EPIPE, errno);
}

TEST_CASE("send(), sendto() and sendmsg() fails"
		" after shutdown with SHUT_RDWR") {
	test_assert_equal(-1, shutdown(sock, SHUT_RDWR));
	test_assert_equal(ENOTCONN, errno);

	test_assert_equal(-1, send(sock, NULL, 0, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, sendto(sock, NULL, 0, 0,
				NULL, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, sendmsg(sock, &msg, 0));
	test_assert_equal(EPIPE, errno);
}

TEST_CASE("recv(), recvfrom() and recvmsg() fails"
		" after shutdown with SHUT_RD") {
	test_assert_equal(-1, shutdown(sock, SHUT_RD));
	test_assert_equal(ENOTCONN, errno);

	test_assert_equal(-1, recv(sock, NULL, 0, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, recvfrom(sock, NULL, 0, 0,
				NULL, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, recvmsg(sock, &msg, 0));
	test_assert_equal(EPIPE, errno);
}

TEST_CASE("recv(), recvfrom() and recvmsg() fails"
		" after shutdown with SHUT_RDWR") {
	test_assert_equal(-1, shutdown(sock, SHUT_RDWR));
	test_assert_equal(ENOTCONN, errno);

	test_assert_equal(-1, recv(sock, NULL, 0, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, recvfrom(sock, NULL, 0, 0,
				NULL, 0));
	test_assert_equal(EPIPE, errno);
	test_assert_equal(-1, recvmsg(sock, &msg, 0));
	test_assert_equal(EPIPE, errno);
}

TEST_CASE("getsockname() and getpeername() fails"
		" on bad descriptor") {
	test_assert_equal(-1, getsockname(BAD_SOCK, &addr, &addrlen));
	test_assert_equal(EBADF, errno);
	test_assert_equal(-1, getpeername(BAD_SOCK, &addr, &addrlen));
	test_assert_equal(EBADF, errno);
}

TEST_CASE("getsockname() and getpeername() fails"
		" on invalid arguments") {
	socklen_t bad_len = BAD_LEN;
	test_assert_equal(-1, getsockname(sock, NULL, &addrlen));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, getsockname(sock, &addr, NULL));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, getsockname(sock, &addr, &bad_len));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, getpeername(sock, NULL, &addrlen));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, getpeername(sock, &addr, NULL));
	test_assert_equal(EINVAL, errno);
	test_assert_equal(-1, getpeername(sock, &addr, &bad_len));
	test_assert_equal(EINVAL, errno);
}

TEST_CASE("getpeername() fails on disconnected soocket") {
	test_assert_equal(-1, getpeername(sock, &addr, &addrlen));
	test_assert_equal(ENOTCONN, errno);
}

static int case_setup(void) {
	static struct iovec iov;

	sock = socket(FAMILY, TYPE, PROTO);
	memset(&addr, 0, sizeof addr);
	addr.sa_family = FAMILY;
	addrlen = sizeof addr;
	memset(&msg, 0, sizeof msg);
	memset(&iov, 0, sizeof iov);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	return sock >= 0 ? 0 : -errno;
}

static int case_teardown(void) {
	return 0 == close(sock) ? 0 : -errno;
}
