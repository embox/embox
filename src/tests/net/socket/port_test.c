/**
 * @file
 * @brief
 *
 * @date 22.06.12
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <netinet/in.h>
#include <net/ip_port.h>
#include <types.h>
#include <errno.h>

EMBOX_TEST_SUITE("Tests for ip_port functions");

#define PORT_FOR_TEST 37

TEST_CASE("Get free port") {
	uint16_t port;

	port = ip_port_get_free(IPPROTO_TCP);
	test_assert_not_zero(port);

	test_assert_zero(ip_port_unlock(IPPROTO_TCP, port));
}

TEST_CASE("Try lock, unlock and lock again a same port") {
	test_assert_zero(ip_port_lock(IPPROTO_TCP, PORT_FOR_TEST));
	test_assert_zero(ip_port_unlock(IPPROTO_TCP, PORT_FOR_TEST));

	test_assert_zero(ip_port_lock(IPPROTO_TCP, PORT_FOR_TEST));
	test_assert_zero(ip_port_unlock(IPPROTO_TCP, PORT_FOR_TEST));
}

TEST_CASE("Contemporaneously try lock a same port for different protocols ") {
	test_assert_zero(ip_port_lock(IPPROTO_TCP, PORT_FOR_TEST));
	test_assert_zero(ip_port_lock(IPPROTO_UDP, PORT_FOR_TEST));

	test_assert_zero(ip_port_unlock(IPPROTO_TCP, PORT_FOR_TEST));
	test_assert_zero(ip_port_unlock(IPPROTO_UDP, PORT_FOR_TEST));
}
