/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "remote.h"

static int remote_fd = -1;

int remote_read(void *buf, size_t count) {
	struct pollfd pfd;
	int ret;

	pfd.fd = remote_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	if (-1 == poll(&pfd, 1, -1)) {
		ret = -errno;
	}
	else {
		ret = read(remote_fd, buf, count);
		if (ret == -1) {
			ret = -errno;
		}
	}
	return ret;
}

int remote_write(const void *buf, size_t count) {
	struct pollfd pfd;
	int ret;

	pfd.fd = remote_fd;
	pfd.events = POLLOUT;
	pfd.revents = 0;

	if (-1 == poll(&pfd, 1, -1)) {
		ret = -errno;
	}
	else {
		ret = write(remote_fd, buf, count);
		if (ret == -1) {
			ret = -errno;
		}
	}
	return ret;
}

int remote_open(const char *host, const char *port) {
	struct sockaddr_in sockaddr;
	in_addr_t in_addr;
	in_port_t in_port;
	int listen_fd;
	char *endptr;
	int ret;

	listen_fd = -1;
	ret = 0;

	in_addr = inet_addr(host);
	if (in_addr == INADDR_BROADCAST) {
		ret = -EINVAL;
		goto out;
	}

	in_port = strtoul(port, &endptr, 10);
	if (*endptr != '\0') {
		ret = -EINVAL;
		goto out;
	}

	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd == -1) {
		ret = -errno;
		goto out;
	}

	if (-1 == setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1},
	                     sizeof(int))) {
		ret = -errno;
		goto out;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(in_port);
	sockaddr.sin_addr.s_addr = in_addr;

	if (-1 == bind(listen_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) {
		ret = -errno;
		goto out;
	}

	if (-1 == listen(listen_fd, 1)) {
		ret = -errno;
		goto out;
	}

	remote_fd = accept(listen_fd, NULL, NULL);
	if (remote_fd == -1) {
		ret = -errno;
	}

out:
	close(listen_fd);
	return ret;
}

void remote_close(void) {
	close(remote_fd);
}
