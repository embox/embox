/**
 * @file
 * @brief
 *
 * @date 24.03.12
 * @author Ilia Vaprol
 */

#include <kernel/prom_printf.h>
#include <net/ip.h>
#include <net/in.h>
#include <net/socket.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(exec);

static int exec(int argc, char **argv) {
	int sockfd, port;
	struct sockaddr_in dst;

	if (argc != 3) {
		prom_printf("Usage: %s <remote ip> <port>", argv[0]);
		return -1;
	}

	if (!inet_aton(argv[1], &dst.sin_addr)) {
		prom_printf("Error...Invalid ip address '%s'", argv[1]);
		return -1;
	}

	if (sscanf(argv[2], "%d", &port) != 1) {
		prom_printf("Error...Invalid port '%s'", argv[2]);
		return -1;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons((unsigned short)port);

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		prom_printf("can't create socket!");
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *)&dst, sizeof dst) < 0) {
		prom_printf("Error... Cant connect to remote address %s:%d\n",
				inet_ntoa(dst.sin_addr), (unsigned short)port);
		close(sockfd);
		return -1;
	}

	prom_printf("Connection established\n");
	close(sockfd);

	return 0;
}
