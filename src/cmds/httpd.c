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

#define SEND_BUFF_SZ 256
#define RECV_BUFF_SZ 128
#define DEFAULT_PAGE "index.html"

struct client_info {
	int sock;
	struct sockaddr_in addr;
	int addr_len;
};

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

static int send_file(char *file, struct client_info *pci) {
	int fd, bytes;
	char buff_out[SEND_BUFF_SZ], *curr;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR("can't open file '%s'\n", file);
		return -1;
	}

	curr = buff_out;
	curr += sprintf(curr, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
	do {
		bytes = read(fd, curr, sizeof buff_out - (curr - buff_out));
		if (bytes < 0) {
			LOG_ERROR("read failed. error=%d\n", bytes);
			close(fd);
			return -1;
		}
		curr += bytes;
		sendto(pci->sock, buff_out, curr - buff_out, 0,
				(struct sockaddr *)&pci->addr, pci->addr_len);
		curr = buff_out;
	} while (bytes > 0);

	close(fd);

	return 0;
}

static int client_process(int client) {
	int bytes_read;
	char buff[RECV_BUFF_SZ];
	char *method, *file;
	struct client_info ci;

	/* Get request */
	ci.sock = client;
	bytes_read = recvfrom(client, buff, sizeof buff - 1, 0, (struct sockaddr *)&ci.addr, &ci.addr_len);
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

	printf("req: method-%s file-%s\n", method, file);
	if (strcmp(method, "GET") == 0) {
		if (strcmp(file, "/") == 0) {
			file = DEFAULT_PAGE;
		}
		if (send_file(file, &ci) < 0) {
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
	int res, host, addr_len;
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
		res = accept(host,(struct sockaddr *)&addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			LOG_ERROR("accept() failed. code=%d", res);
			close(host);
			return res;
		}
		client_process(res);
	}

	close(host);

	return ENOERR;
}
