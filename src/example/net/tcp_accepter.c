/**
 * @file
 * @brief
 *
 * @date 17.03.12
 * @author Ilia Vaprol
 */

#include <stdio.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(exec);

#define LISTENING_PORT	18

static int exec(int argc, char **argv) {
	int res, sockfd;
	struct sockaddr_in src, dst;
	socklen_t dst_addr_len;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		printf("can't create socket!");
		return -1;
	}

	src.sin_family = AF_INET;
	src.sin_port= htons(LISTENING_PORT);
	/* src.port_type = TCP_PORT; */
	src.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sockfd, (struct sockaddr *)&src, sizeof src);
	if (res < 0) {
		printf("sock can't bind!");
		return res;
	}

	res = listen(sockfd, 1);
	if (res < 0) {
		printf("sock can't listen!");
		return res;
	}

	printf("Hello. I'm tcp_accepter. I will accept all connections to %s:%d address\n",
			inet_ntoa(src.sin_addr), LISTENING_PORT);
	while (1) {
		res = accept(sockfd, (struct sockaddr *)&dst, &dst_addr_len);
		if (res < 0) {
			printf("no one can't be accepted\n");
			return res;
 		}
		printf("\nclient from %s:%d at %d socket\n\n",
				inet_ntoa(dst.sin_addr), ntohs(dst.sin_port), res);
	}

	return 0;
}
