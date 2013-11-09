/**
 * @file
 * @brief
 *
 * @date 01.11.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <embox/test.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

EMBOX_TEST_SUITE("netdb/getaddrinfo tests");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

#define ANY 0
#define NODE_NAME "127.0.0.1"
#define NODE_ADDR INADDR_LOOPBACK
#define SERV_NAME "5051"
#define SERV_PORT 5051
#define FAMILY    AF_INET
#define SOCKTYPE  SOCK_STREAM
#define PROTOCOL  IPPROTO_TCP

#define BAD_FAMILY   -1
#define BAD_SOCKTYPE -1
#define BAD_PROTOCOL -1
#define BAD_NAME     "bad_name"

#define GET_AI(nodename, servname)                        \
	((ret = getaddrinfo(nodename, servname, hints, &res)) \
		? (ai = res = NULL, ret) : (ai = res, ret))

#define EQUAL_AI(family, socktype, protocol, addr)           \
	((ai->ai_flags == 0) && (ai->ai_family == family)        \
		&& (ai->ai_socktype == socktype)                     \
		&& (ai->ai_protocol == protocol)                     \
		&& (ai->ai_addrlen == sizeof addr)                   \
		&& (0 == memcmp(ai->ai_addr, &addr, ai->ai_addrlen)) \
		&& (ai->ai_canonname == NULL))

#define NEXT_AI() \
	(ai = ai->ai_next)

/* Private variables */
int ret;
static struct addrinfo hints_, *res;
static struct {
	struct sockaddr_in in;
	struct sockaddr_in6 in6;
} addr;

/* User variables */
static struct addrinfo *hints, *ai;

TEST_CASE("getaddrinfo() fails with EAI_NONAME if node and"
		" service unspecified") {
	test_assert_equal(EAI_NONAME, GET_AI(NULL, NULL));
}

TEST_CASE("getaddrinfo() fails with EAI_FAMILY if family not"
		" supported") {
	hints->ai_family = BAD_FAMILY;
	test_assert_equal(EAI_FAMILY, GET_AI(NODE_NAME, SERV_NAME));
}

TEST_CASE("getaddrinfo() fails with EAI_SOCKTYPE if socket type"
		" not supported") {
	hints->ai_socktype = BAD_SOCKTYPE;
	test_assert_equal(EAI_SOCKTYPE, GET_AI(NODE_NAME, SERV_NAME));
}

TEST_CASE("getaddrinfo() fails with EAI_SERVICE if service not"
		" supported for socket type") {
	hints->ai_protocol = BAD_PROTOCOL;
	test_assert_equal(EAI_SERVICE, GET_AI(NODE_NAME, SERV_NAME));
}

TEST_CASE("getaddrinfo() fails with EAI_SOCKTYPE if socket type"
		" not supported with specified protocol") {
	hints->ai_socktype = SOCKTYPE;
	hints->ai_protocol = BAD_PROTOCOL;
	test_assert_equal(EAI_SOCKTYPE, GET_AI(NODE_NAME, SERV_NAME));
}

TEST_CASE("getaddrinfo() fails with EAI_SERVICE on bad service"
		" name") {
	test_assert_equal(EAI_SERVICE, GET_AI(NODE_NAME, BAD_NAME));
}

TEST_CASE("getaddrinfo() fails with EAI_NONAME on bad node"
		" name") {
	test_assert_equal(EAI_NONAME, GET_AI(BAD_NAME, SERV_NAME));
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" nodename, servname, family, socktype and protocol") {
	hints->ai_family = FAMILY;
	hints->ai_socktype = SOCKTYPE;
	hints->ai_protocol = PROTOCOL;
	test_assert_equal(0, GET_AI(NODE_NAME, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(FAMILY, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" nodename, servname, family and socktype") {
	hints->ai_family = FAMILY;
	hints->ai_socktype = SOCKTYPE;
	test_assert_equal(0, GET_AI(NODE_NAME, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(FAMILY, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" nodename, servname, family and protocol") {
	hints->ai_family = FAMILY;
	hints->ai_protocol = PROTOCOL;
	test_assert_equal(0, GET_AI(NODE_NAME, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(FAMILY, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" nodename, servname and family") {
	hints->ai_family = FAMILY;
	test_assert_equal(0, GET_AI(NODE_NAME, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(FAMILY, SOCK_STREAM, IPPROTO_TCP,
				addr.in));
	test_assert_not_null(NEXT_AI());
	test_assert_true(EQUAL_AI(FAMILY, SOCK_DGRAM, IPPROTO_UDP,
				addr.in));
	test_assert_not_null(NEXT_AI());
	test_assert_true(EQUAL_AI(FAMILY, SOCK_RAW, 0, addr.in));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" servname, socktype and loopback-address") {
	hints->ai_socktype = SOCKTYPE;
	test_assert_equal(0, GET_AI(NULL, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(AF_INET, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_not_null(NEXT_AI());

	addr.in6.sin6_family = AF_INET6;
	memcpy(&addr.in6.sin6_addr, &in6addr_loopback,
			sizeof addr.in6.sin6_addr);
	addr.in6.sin6_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(AF_INET6, SOCKTYPE, PROTOCOL,
				addr.in6));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with specified"
		" servname, flag AI_PASSIVE, socktype and any-address") {
	hints->ai_flags |= AI_PASSIVE;
	hints->ai_socktype = SOCKTYPE;
	test_assert_equal(0, GET_AI(NULL, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(AF_INET, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_not_null(NEXT_AI());

	addr.in6.sin6_family = AF_INET6;
	memcpy(&addr.in6.sin6_addr, &in6addr_any,
			sizeof addr.in6.sin6_addr);
	addr.in6.sin6_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(AF_INET6, SOCKTYPE, PROTOCOL,
				addr.in6));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() ignores AI_PASSIVE flag if nodename is"
		" specified") {
	hints->ai_flags |= AI_PASSIVE;
	hints->ai_socktype = SOCKTYPE;
	test_assert_equal(0, GET_AI(NODE_NAME, SERV_NAME));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = htons(SERV_PORT);
	test_assert_true(EQUAL_AI(FAMILY, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_null(NEXT_AI());
}

TEST_CASE("getaddrinfo() returns addrinfo with port 0 if"
		" servname not specified") {
	hints->ai_family = FAMILY;
	hints->ai_socktype = SOCKTYPE;
	test_assert_equal(0, GET_AI(NODE_NAME, NULL));

	addr.in.sin_family = AF_INET;
	addr.in.sin_addr.s_addr = htonl(NODE_ADDR);
	addr.in.sin_port = 0;
	test_assert_true(EQUAL_AI(FAMILY, SOCKTYPE, PROTOCOL,
				addr.in));
	test_assert_null(NEXT_AI());
}

static int case_setup(void) {
	memset(&hints_, 0, sizeof hints_);
	hints_.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
	hints = &hints_;
	memset(&addr, 0, sizeof addr);

	return 0;
}

static int case_teardown(void) {
	if (res != NULL) {
		freeaddrinfo(res);
	}

	return 0;
}
