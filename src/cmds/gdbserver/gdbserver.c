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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <debug/gdbstub.h>
#include <framework/cmd/api.h>
#include <framework/cmd/types.h>

static int remote_fd;

static void get_packet(char *dst, size_t nbyte) {
	char buf[2];

	do {
		read(remote_fd, buf, 1);
	} while (buf[0] != '$');

	while (nbyte--) {
		read(remote_fd, buf, 1);
		*dst++ = buf[0];
		if (buf[0] == '#') {
			break;
		}
	}
	read(remote_fd, buf, 2);
}

static void put_packet(const char *src, size_t nbyte) {
	write(remote_fd, src, nbyte);
}

static int prepare_connection(const char *host_port) {
	struct sockaddr_in sockaddr;
	in_addr_t addr;
	in_port_t port;
	char *delim;
	char *endptr;
	int listen_fd;
	int ret;

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

	ret = accept(listen_fd, NULL, NULL);
	if (ret == -1) {
		ret = -errno;
	}

out:
	shutdown(listen_fd, SHUT_RDWR);
	close(listen_fd);
	return ret;
}

int main(int argc, char *argv[]) {
	struct gdbstub_state state;
	const struct cmd *cmd;
	int ret;

	ret = 0;

	if (argc < 3) {
		ret = -EINVAL;
		goto out;
	}

	cmd = cmd_lookup(argv[2]);
	if (cmd == NULL) {
		ret = -ENOENT;
		goto out;
	}

	remote_fd = prepare_connection(argv[1]);
	if (remote_fd < 0) {
		ret = remote_fd;
		goto out;
	}

	state.connected = false;
	state.ops.get_packet = get_packet;
	state.ops.put_packet = put_packet;

	gdb_start_debugging(&state, cmd->exec);
	cmd_exec(cmd, argc - 2, &argv[2]);
	gdb_stop_debugging(&state);

out:
	if (ret == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [ARGS ...]\n", argv[0]);
	}
	shutdown(remote_fd, SHUT_RDWR);
	close(remote_fd);
	return ret;
}
