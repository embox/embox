/**
 * @file
 * @brief
 *
 * @date 31.01.12
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

EMBOX_TEST_SUITE("network/socket/inet test");

TEST_CASE("Create raw IP (internet) socket") {
	int sockfd;

	sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
	test_assert(sockfd >= 0);

	close(sockfd);
}

TEST_CASE("Create raw ICMP socket") {
	int sockfd;

	sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	test_assert(sockfd);

	close(sockfd);
}
