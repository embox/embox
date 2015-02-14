
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "cnc_ipc.h"

#define SSE_HEADER "data: \""
#define SSE_HEADER_LEN strlen(SSE_HEADER)
#define SSE_FOOTER "\"\n\n"
#define SSE_FOOTER_LEN strlen(SSE_FOOTER)
#define SSE_SERVICE_LEN (SSE_HEADER_LEN + SSE_FOOTER_LEN)

static int cnc_ipc_get_input_fd(void) {
	char *cnc_in;
	if (!(cnc_in = getenv(CNCIPC_IN))) {
		printf("Error: IPC not found");
		return 0;
	}

	return atoi(cnc_in);
}

int main(int argc, char *argv[]) {
	const int sock = STDOUT_FILENO;
	const int ipc_in = cnc_ipc_get_input_fd();
	char buf[128];
	struct pollfd fds[2];
	bool sock_conn, ipc_has_data;
	int buf_written;

	fds[0].fd = ipc_in;
	fds[0].events = POLLIN | POLLERR;
	fds[1].fd = sock;
	fds[1].events = POLLERR;


	buf_written = snprintf(buf, sizeof(buf),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n");

	ipc_has_data = false;
	sock_conn = 0 < write(sock, buf, buf_written);
	while (sock_conn) {
		if (poll(fds, 2, -1) > 0) {
			sock_conn = !(fds[1].revents & POLLERR);
			ipc_has_data = fds[0].revents & POLLIN;
		}

		if (sock_conn && ipc_has_data) {
			char *buf_data = buf + SSE_HEADER_LEN;
			strcpy(buf, SSE_HEADER);
			buf_written = read(ipc_in, buf_data, sizeof(buf) - SSE_SERVICE_LEN);
			assert(buf_written > 0);
			strcpy(buf_data + buf_written, SSE_FOOTER);
			sock_conn = 0 < write(sock, buf, buf_written + SSE_SERVICE_LEN);
		}
	}

	return 0;
}
