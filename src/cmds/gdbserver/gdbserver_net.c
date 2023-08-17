/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.01.23
 */
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

ssize_t gdbserver_read(int fd, char *dst, size_t nbyte) {
	ssize_t nread;
	char *ptr;

	for (;;) {
		nread = read(fd, dst, nbyte);
		if (nread > 0) {
			ptr = memchr(dst, '$', nread);
			if (!ptr) {
				continue;
			}
			nread -= ptr - dst;
			memcpy(dst, ptr, nread);
		}
		break;
	}
	return nread;
}

ssize_t gdbserver_write(int fd, const char *src, size_t nbyte) {
	return write(fd, src, nbyte);
}

int gdbserver_prepare_connection(const char *host_port) {
	struct sockaddr_in sockaddr;
	in_addr_t addr;
	in_port_t port;
	char *delim;
	char *endptr;
	int listen_fd;
	int ret;
	int fd;

	listen_fd = -1;

	delim = strchr(host_port, ':');
	if ((delim == NULL) || (delim[1] == '\n')) {
		ret = -EINVAL;
		goto out;
	}
	*delim = '\0';

	addr = inet_addr(host_port);
	if (addr == INADDR_BROADCAST) {
		ret = -EINVAL;
		goto out;
	}

	port = strtoul(delim + 1, &endptr, 10);
	if (*endptr != '\0') {
		ret = -EINVAL;
		goto out;
	}

	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd == -1) {
		ret = -errno;
		goto out;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = addr;

	if (-1 == bind(listen_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) {
		ret = -errno;
		goto out;
	}

	if (-1 == listen(listen_fd, 1)) {
		ret = -errno;
		goto out;
	}

	printf("Listening on port %hu\n", port);

	ret = fd = accept(listen_fd, NULL, NULL);
	if (fd == -1) {
		ret = -errno;
	}

out:
	close(listen_fd);

	return ret;
}
