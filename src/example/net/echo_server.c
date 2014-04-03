/**
 * @file
 * @brief Echo server for UDP and TCP protocol
 *
 * @date 16.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <util/math.h>

#include <unistd.h>

#include <framework/example/self.h>
EMBOX_EXAMPLE(exec);

#define ECHO_PORT 700
#define BUFF_SZ 16384

static char buff[BUFF_SZ];

/**
 * Initializer of udp socket
 */
static int sock_udp_init(int *out_sock) {
	int ret, res, sock;
	struct sockaddr_in addr;

	assert(out_sock != NULL);

	/**
	 * Make udp socket
	 */
	res = socket(AF_INET, SOCK_DGRAM, 0);
	if (res == -1) { ret = -errno; perror("socket"); return ret; }
	sock = res;

	/**
	 * Bind to local address
	 */
	addr.sin_family = AF_INET;
	addr.sin_port= htons(ECHO_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&addr, sizeof addr);
	if (res == -1) { ret = -errno; perror("bind"); close(sock); return ret; }

	*out_sock = sock;

	return 0;
}

/**
 * Handler for udp socket
 */
static int sock_udp_hnd(int sock) {
	int ret;
	char *data;
	struct sockaddr addr;
	socklen_t addr_len;
	ssize_t bytes, bytes_left;

	/**
	 * Receive data
	 */
	addr_len = sizeof addr;
	bytes = recvfrom(sock, &buff, sizeof buff, 0, &addr, &addr_len);
	if (bytes == -1) { ret = -errno; perror("recvfrom"); return ret; }

	/**
	 * Send back
	 */
	data = &buff[0];
	bytes_left = bytes;
	while (bytes_left > 0) {
		bytes = sendto(sock, data, bytes_left, 0, &addr, addr_len);
		if (bytes == -1) { ret = -errno; perror("sendto"); return ret; }

		data += bytes;
		bytes_left -= bytes;
	}

	return 0;
}

/**
 * Initializer of tcp socket
 */
static int sock_tcp_init(int *out_sock) {
	int ret, res, sock;
	struct sockaddr_in addr;

	assert(out_sock != NULL);

	/**
	 * Make udp socket
	 */
	res = socket(AF_INET, SOCK_STREAM, 0);
	if (res == -1) { perror("socket"); return -errno; }
	sock = res;

	/**
	 * Bind to local address
	 */
	addr.sin_family = AF_INET;
	addr.sin_port= htons(ECHO_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&addr, sizeof addr);
	if (res == -1) { ret = -errno; perror("bind"); close(sock); return ret; }

	/**
	 * Mark as listening
	 */
	res = listen(sock, 1);
	if (res == -1) { ret = -errno; perror("listen"); close(sock); return ret; }

	*out_sock = sock;

	return 0;
}

/**
 * Handler for tcp socket
 */
static int sock_tcp_hnd(int sock) {
	int ret, res;
	char *data;
	ssize_t bytes, bytes_left;

	/**
	 * Accept new connection
	 */
	res = accept(sock, NULL, NULL);
	if (res == -1) { ret = -errno; perror("accept"); return ret; }
	sock = res;

	/**
	 * Start echo for this socket
	 */
	while (1) {
		/**
		 * Try to receive anything
		 */
		bytes = recv(sock, &buff, sizeof buff, 0);
		if (bytes == -1) { ret = -errno; perror("recv"); return ret; }
		else if (bytes == 0) break;

		/**
		 * Send it back
		 */
		data = &buff[0];
		bytes_left = bytes;
		while (bytes_left > 0) {
			bytes = send(sock, data, bytes_left, 0);
			if (bytes == -1) { ret = -errno; perror("send"); return ret; }

			data += bytes;
			bytes_left -= bytes;
		}
	}

	/**
	 * That is all for now
	 */
	res = close(sock);
	if (res == -1) { ret = -errno; perror("close"); return ret; }

	return 0;
}

/**
 * Main tcp and udp sockets handlers
 */
static int socks_handler(int sock_udp, int sock_tcp) {
	int ret, res, nfds;
	fd_set fds, readfds;

	/**
	 * Fill select structure
	 */
	FD_ZERO(&fds);
	FD_SET(sock_udp, &fds);
	FD_SET(sock_tcp, &fds);
	nfds = max(sock_udp, sock_tcp) + 1;

	while (1) {
		/**
		 * Make copy
		 */
		memcpy(&readfds, &fds, sizeof fds);

		/**
		 * Waiting any actions from sockets
		 */
		res = select(nfds, &readfds, NULL, NULL, NULL);
		if (res == -1) { ret = -errno; perror("select"); return ret; }

		/**
		 * Handling action from udp socket
		 */
		if (FD_ISSET(sock_udp, &readfds)) {
			ret = sock_udp_hnd(sock_udp);
			if (ret != 0) {
				return ret;
			}
		}
		/**
		 * Handling action from tcp socket
		 */
		if (FD_ISSET(sock_tcp, &readfds)) {
			ret = sock_tcp_hnd(sock_tcp);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

static int exec(int argc, char *argv[]) {
	int ret, res, sock_udp, sock_tcp;

	sock_udp = sock_tcp = -1; /* XXX required for -O2 */

	/**
	 * Make udp socket
	 */
	ret = sock_udp_init(&sock_udp);
	if (ret != 0) return ret;

	/**
	 * Make tcp socket
	 */
	ret = sock_tcp_init(&sock_tcp);
	if (ret != 0) { close(sock_udp); return ret; }

	/**
	 * Handling requests
	 */
	ret = socks_handler(sock_udp, sock_tcp);
	if (ret != 0) { close(sock_udp); close(sock_tcp); return ret; }

	/**
	 * Close udp socket
	 */
	res = close(sock_udp);
	if (res == -1) { ret = -errno; perror("close"); close(sock_tcp); return ret; }

	/**
	 * Close tcp socket
	 */
	res = close(sock_tcp);
	if (res == -1) { ret = -errno; perror("close"); return ret; }

	return 0;
}
