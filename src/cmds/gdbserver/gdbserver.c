/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.01.23
 */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <lib/gdb.h>
#include <framework/cmd/api.h>
#include <framework/cmd/types.h>

static int gdb_fd;

static ssize_t get_packet(char *dst, size_t nbyte) {
	ssize_t nread;
	char *ptr;

	for (;;) {
		nread = read(gdb_fd, dst, nbyte);
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

static ssize_t get_packet2(char *dst, size_t nbyte) {
	ssize_t nread;

	do {
		read(gdb_fd, dst, 1);
	} while (*dst != '$');

	for (nread = 1; nread < nbyte; nread++) {
		read(gdb_fd, ++dst, 1);
		if (*dst == '#') {
			break;
		}
	}
	return nread;
}

static ssize_t put_packet(const char *src, size_t nbyte) {
	return write(gdb_fd, src, nbyte);
}

static int prepare_tcp(const char *host_port) {
	struct sockaddr_in sockaddr;
	in_addr_t addr;
	in_port_t port;
	char *delim;
	char *endptr;
	int listen_fd;
	int ret;

	listen_fd = -1;
	ret = 0;

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

	gdb_fd = accept(listen_fd, NULL, NULL);
	if (gdb_fd == -1) {
		ret = -errno;
	}

out:
	close(listen_fd);
	return ret;
}

static int prepare_serial(const char *tty) {
	struct termios t;

	printf("Remote debugging using %s\n", tty);

	gdb_fd = open(tty, O_RDWR);
	if (gdb_fd == -1) {
		return -errno;
	}

	if (-1 == tcgetattr(gdb_fd, &t)) {
		return -errno;
	}

	t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	t.c_oflag &= ~(OPOST);
	t.c_cflag |= (CS8);
	t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (-1 == tcsetattr(gdb_fd, TCSANOW, &t)) {
		return -errno;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	struct gdb_ops gdb_ops;
	const struct cmd *cmd;
	int ret;

	gdb_fd = -1;

	if (argc < 3) {
		ret = -EINVAL;
		goto out;
	}

	cmd = cmd_lookup(argv[2]);
	if (cmd == NULL) {
		ret = -ENOENT;
		goto out;
	}

	if (isdigit(*argv[1])) {
		gdb_ops.get_packet = get_packet;
		gdb_ops.put_packet = put_packet;
		ret = prepare_tcp(argv[1]);
	}
	else {
		gdb_ops.get_packet = get_packet2;
		gdb_ops.put_packet = put_packet;
		ret = prepare_serial(argv[1]);
	}
	if (ret < 0) {
		goto out;
	}

	gdb_prepare(&gdb_ops);
	{
		gdb_set_bpt(cmd->exec);
		cmd_exec(cmd, argc - 2, &argv[2]);
	}
	gdb_cleanup();

out:
	if (ret == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [ARGS ...]\n", argv[0]);
	}
	close(gdb_fd);
	return ret;
}
