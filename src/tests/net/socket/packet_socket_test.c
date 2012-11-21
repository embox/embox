/**
 * @file
 *
 * @brief
 *
 * @date 31.01.2012
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

EMBOX_TEST_SUITE("network/socket/packet test");

TEST_CASE("Create raw PF_PACKET socket") {
	int sockfd;

	sockfd = socket(PF_PACKET, SOCK_RAW, 0);
	test_assert(sockfd >= 0);

	close(sockfd);
}
