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

#define BUFF_SZ 256
#define FILE_READ_CHUNK_SZ 64

static int parse_req(char *buff, char **method, char **file) {
	char *tmp;

	/* Find method */
	*method = buff;
	for (tmp = *method; *tmp != ' '; ++tmp) {
		if (*tmp == '\0') {
			LOG_ERROR("no such file\n");
			return -1;
		}
	}
	*tmp = '\0'; /* set end of method */

	/* Find file */
	*file = tmp + 1;
	for (tmp = *file; *tmp != ' '; ++tmp) {
		if (*tmp == '\0') {
			LOG_ERROR("no such version\n");
			return -1;
		}
	}
	*tmp = '\0'; /* set end of method */

	return 0;
}

static int send_file(char *file, int client, struct sockaddr_in addr, int addr_len) {
	int fd, bytes_read;
	char buff[BUFF_SZ], *curr;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR("can't open file '%s'\n", file);
		close(client);
		return -1;
	}
	curr = buff;
	curr += sprintf(curr, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
	while ((bytes_read = read(fd, curr, FILE_READ_CHUNK_SZ)) > 0) { /* FIXME check the boundaries */
		curr += bytes_read;
	}
	if (bytes_read < 0) {
		close(fd);
		return -1;
	}
	sendto(client, buff, curr - buff, 0,
				(struct sockaddr *)&addr, addr_len);
	return 0;
}

static int client_process(int client) {
	int bytes_read, addr_len;
	char buff[BUFF_SZ];
	char *method, *file;
	struct sockaddr_in addr;

	/* Get request */
	bytes_read = recvfrom(client, buff, sizeof buff - 1, 0, (struct sockaddr *)&addr, &addr_len);
	if (bytes_read < 0) {
		LOG_ERROR("recvfrom() failed. code=%d\n", bytes_read);
		close(client);
		return bytes_read;
	}
	buff[bytes_read] = '\0';

	/* Parse request */
	if (parse_req(buff, &method, &file) != 0) {
		close(client);
		return -1;
	}

	if (strcmp(method, "GET") == 0) {
		if (send_file(file, client, addr, addr_len) < 0) {
			close(client);
			return -1;
		}
	}
	else { /* not suppported */
		LOG_ERROR("method '%s' not supported\n", method);
		close(client);
		return -ENOSUPP;
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
