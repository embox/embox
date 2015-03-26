/**
 * @file
 * @brief Speed test
 *
 * @date 07.01.14
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <util/math.h>
#include <unistd.h>

#define BUFF_SZ 16384

static char buff[BUFF_SZ];

static int client_tcp(size_t nbyte, struct sockaddr_in *in) {
	int sock, i;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		perror("socket");
		return -errno;
	}

	if (-1 == connect(sock, (struct sockaddr *)in, sizeof *in)) {
		perror("connect");
		close(sock);
		return -errno;
	}

	memset(&buff[0], 0, sizeof buff);
	for (i = 1; i < 11; ++i) buff[i] = i;
	for (i = 1; i < 11; ++i) buff[sizeof buff - 1 - i] = i;

	while (nbyte != 0) {
		ssize_t ret;

		ret = send(sock, &buff[0], min(sizeof buff, nbyte), 0);
		if (ret == -1) {
			perror("send");
			close(sock);
			return -errno;
		}

		++buff[0];
		--buff[sizeof buff - 1];
		nbyte -= ret;
	}

	if (-1 == close(sock)) {
		perror("close");
		return -errno;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	int opt, count;
	size_t nbyte;
	char *protocol, *address, *port, *tmp;
	struct sockaddr_in in;

	nbyte = 0;
	memset(&in, 0, sizeof in);
	in.sin_family = AF_INET;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "K:M:G:h"))) {
		switch (opt) {
		case '?':
		case 'h':
			printf("%s: [-K <bytes K>] [-M <bytes M>] [-G <bytes G>]"
					"protocol://address:port\n", argv[0]);
			return 0;
		case 'K':
		case 'M':
		case 'G':
			if (1 != sscanf(optarg, "%d", &count)) {
				printf("%s: bad number\n", argv[0]);
				return -EINVAL;
			}
			nbyte += count * (
					opt == 'K' ? 1024
					: opt == 'M' ? 1024 * 1024
					: 1024 * 1024 * 1024);
			break;
		}
	}

	if (optind >= argc) {
		printf("%s: target not specified\n", argv[0]);
		return -EINVAL;
	}
	else {
		protocol = argv[optind];
		tmp = strstr(protocol, "://");
		if (tmp == NULL) {
			printf("%s: bad target format '%s`\n",
					argv[0], argv[optind]);
			return -EINVAL;
		}
		*tmp = '\0';
		address = tmp + 3;
		tmp = strchr(address, ':');
		if (tmp == NULL) {
			printf("%s: bad target format '%s`\n",
					argv[0], argv[optind]);
			return -EINVAL;
		}
		*tmp = '\0';
		if (0 == inet_aton(address, &in.sin_addr)) {
			printf("%s: invalid address '%s`\n",
					argv[0], address);
			return -EINVAL;
		}
		port = tmp + 1;
		if (1 != sscanf(port, "%hu", &in.sin_port)) {
			printf("%s: invalid port '%s`\n",
					argv[0], port);
			return -EINVAL;
		}
		in.sin_port = htons(in.sin_port);
	}

	if (0 == strcmp(protocol, "tcp")) {
		return client_tcp(nbyte, &in);
	}

	printf("%s: protocol '%s` not supported\n",
			argv[0], protocol);
	return -EINVAL;
}
