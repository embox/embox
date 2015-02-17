
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include <sys/uio.h>
#include <util/math.h>

#include "cnc_ipc.h"

#define SSE_HEADER "data: \""
#define SSE_FOOTER "\"\n\n"

static int cnc_ipc_get_input_fd(void) {
	char *cnc_in;
	if (!(cnc_in = getenv(CNCIPC_IN))) {
		printf("Error: IPC not found");
		return 0;
	}

	return atoi(cnc_in);
}

static int memcpy_iov(void *dst, size_t dst_len, struct iovec *iov, int iovlen) {
	int i_iov;
	void *bp;
	int bp_len;

	bp = dst;
	bp_len = dst_len;
	for (i_iov = 0; i_iov < iovlen; ++i_iov) {
		const int to_write = min(iov[i_iov].iov_len, bp_len);
		memcpy(bp, iov[i_iov].iov_base, to_write);
		bp += to_write;
		bp_len -= to_write;
	}

	return bp - dst;
}

int main(int argc, char *argv[]) {
	const int sock = STDOUT_FILENO;
	const int ipc_in = cnc_ipc_get_input_fd();
	char buf[64], out_buf[64];
	struct pollfd fds[2];
	struct iovec iov[3];
	bool sock_conn, ipc_has_data;
	int buf_len;

	fds[0].fd = ipc_in;
	fds[0].events = POLLIN | POLLERR;
	fds[1].fd = sock;
	fds[1].events = POLLERR;

	buf_len = snprintf(buf, sizeof(buf),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n");

	iov[0].iov_base = SSE_HEADER;
	iov[0].iov_len = strlen(SSE_HEADER);
	iov[2].iov_base = SSE_FOOTER;
	iov[2].iov_len = strlen(SSE_FOOTER);

	ipc_has_data = false;
	sock_conn = 0 < write(sock, buf, buf_len);
	while (sock_conn) {
		if (poll(fds, 2, -1) > 0) {
			sock_conn = !(fds[1].revents & POLLERR);
			ipc_has_data = fds[0].revents & POLLIN;
		}

		if (sock_conn && ipc_has_data) {
			int buf_o;
			buf_len = read(ipc_in, buf, sizeof(buf));
			assert(buf_len > 0);

			buf_o = 0;
			while (buf_o < buf_len) {
				char *eom = memchr(buf + buf_o, '\n', buf_len - buf_o);
				const int body_len = eom ? eom - (buf + buf_o) : buf_len - buf_o;
				int write_len;

				iov[1].iov_base = buf + buf_o;
				iov[1].iov_len = body_len;

				write_len = memcpy_iov(out_buf, sizeof(out_buf), iov, 3);
				sock_conn = 0 < write(STDOUT_FILENO, out_buf, write_len);
				if (!sock_conn) {
					break;
				}

				buf_o += body_len + 1;
			}
		}
	}

	return 0;
}
