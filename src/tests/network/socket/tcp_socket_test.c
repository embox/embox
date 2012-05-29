/**
 * @file
 *
 * @brief
 *
 * @date 29.05.2012
 * @author Anton Bondarev
 */
#include <embox/test.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ip.h>
#include <unistd.h>

EMBOX_TEST_SUITE("network/socket/inet test");

TEST_CASE("Create TCP socket") {
	int sockfd;
	struct sockaddr_in dst;



	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	test_assert(sockfd > 0);

	dst.sin_family = AF_INET;
	dst.sin_port = htons((unsigned short)12345);

	test_assert(connect(sockfd, (struct sockaddr *)&dst, sizeof(dst)));

	close(sockfd);
}
