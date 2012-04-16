/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <embox/cmd.h>
#include <err.h>
#include <errno.h>

EMBOX_CMD(exec);

#define FILE_BUF_SZ 1024
#define REQ_BUF_SZ 128
#define FILE_READ_CHUNK_SZ 2

static int client_process(int client) {
	int bytes_read;
	char file_buf[FILE_BUF_SZ], req_buf[REQ_BUF_SZ];
	char *f = file_buf;
	struct sockaddr_in addr;
	int addr_len;

	f += sprintf(file_buf,
			"HTTP/1.0 200 OK\n"
			"Content-Type: text/html\n"
			"\n");
#if 0
	int fd = open(argv[1], 0);
	if (fd < 0) {
		printf("Cannot open file '%s'\n", argv[1]);
		return -1;
	}
	while ((bytes_read = read(fd, f, FILE_READ_CHUNK_SZ)) > 0) {
		f += bytes_read;
	}
	close(fd);
#else
	f += sprintf(f, "<html><p>Hello World!</p></html>");
#endif
	do {
		bytes_read = recvfrom(client, req_buf, 3, 0,
				(struct sockaddr *)&addr, &addr_len);
		if (bytes_read < 0) {
			printf("recvfrom fail\n");
			close(client);
			return bytes_read;
		}
	} while ((bytes_read >= 3) && (strncmp(req_buf, "GET", 3) != 0));
	if (bytes_read > 0) {
		sendto(client, file_buf, f - file_buf, 0,
				(struct sockaddr *)&addr, addr_len);
	}
	close(client);
	return ENOERR;
}

static int exec(int argc, char **argv) {
	int res, host, client, addr_len;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port= htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host < 0) {
		LOG_ERROR("can't create socket. code=%d\n", host);
		return host;
	}

	res = bind(host, (struct sockaddr *)&addr, sizeof(addr));
	if (res < 0) {
		LOG_ERROR("bind() failed. code=%d", res);
		return res;
	}

	res = listen(host, 1);
	if (res < 0) {
		LOG_ERROR("listen() failed. code=%d", res);
		return res;
	}

	while (1) {
		client = accept(host,(struct sockaddr *)&addr, &addr_len);
		if (client < 0) {
			/* error code in client, now */
			LOG_ERROR("accept() failed. code=%d", client);
			close(host);
			return client;
		}
		client_process(client);
	}

	close(host);

	return ENOERR;
}
