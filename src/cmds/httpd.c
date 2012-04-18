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

#define FILE_NAME_MAX 32
#define BUFF_SZ       512
#define DEFAULT_PAGE  "index.html"

enum http_method {
	HTTP_METHOD_UNKNOWN,
	HTTP_METHOD_GET
};

struct client_info {
	int sock;                /* socket for client connection*/
	struct sockaddr_in addr; /* client address */
	int addr_len;            /* address length */
	enum http_method method; /* method in request */
	char *req_file;          /* client want open this page */
};


static int http_method_get(struct client_info *info, char *buff, size_t len) {
	int fd, bytes, bytes_read;
	char *curr;

	if (strcmp(info->req_file, "/") == 0) {
		info->req_file = DEFAULT_PAGE;
	}

	curr = buff;
	fd = open(info->req_file, O_RDONLY);
	if (fd < 0) {
		curr += sprintf(curr,
				"HTTP/1.0 404 OK\n"
				"Content-Type: text/html\n"
				"\n"
				"<html>"
				"<head>"
				"<title>404 Not Found</title>"
				"</head>"
				"<body>"
				"<center><h1>404</h1></center>"
				"</body>"
				"</html>"
				);
		printf("Send 404\n");
		sendto(info->sock, buff, curr - buff, 0,
				(struct sockaddr *)&info->addr, info->addr_len);
		printf("Done Send 404\n");
		return -1;
	}

	curr += sprintf(curr, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
	do {
		bytes_read = sizeof buff - (curr - buff);
		bytes = read(fd, curr, bytes_read);
		if (bytes < 0) {
			close(fd);
			return -1;
		}
		curr += bytes;
		if (sendto(info->sock, buff, curr - buff, 0,
				(struct sockaddr *)&info->addr, info->addr_len) < 0 ) {
			close(fd);
			return -1;
		}
		curr = buff;
	} while (bytes == bytes_read);

	close(fd);

	return 0;
}

static int get_line(struct client_info *pci, char *buff, size_t len) {
	int res;

	/* XXX len must be greater or equal 2 */
	do {
		res = recvfrom(pci->sock, buff, sizeof(char), 0, (struct sockaddr *)&pci->addr, &pci->addr_len);
	} while ((res == 1) && (*buff++ != '\n') && (--len > 1));
	*buff = '\0';
	return res;
}

static void client_process(int sock) {
	int res;
	char buff[BUFF_SZ], req_file[FILE_NAME_MAX], *tmp;
	struct client_info ci;

	memset(&ci, 0, sizeof ci);

	ci.sock = sock;

	/* Parse HTTP header */
	res = get_line(&ci, buff, sizeof buff);
	if (res < 0) {
		close(sock);
		return;
	}
	printf("httpd head: '%s'\n", buff);
	if (strncmp(buff, "GET ", 4) == 0) {
		/* set method */
		ci.method = HTTP_METHOD_GET;
		/* set file */
		ci.req_file = req_file;
		for (tmp = buff + 4; (*tmp != ' ') && (*tmp != '?'); ++tmp) {
			if (*tmp == '\0') {
				close(sock);
				return;
			}
		}
		*tmp = '\0';
		strcpy(req_file, buff + 4); // save filename
	}
//	else if (strncmp(buff, "POST ", 5) == 0)
	else {
		printf("httpd warning: unknown method '%s'", buff);
	}
	do {
		res = get_line(&ci, buff, sizeof buff);
		printf("httpd opts: '%s'\n", buff);
		if (strcmp(buff, "\n")) { break; } /* end header section */
		else if (strncmp(buff, "Host:", 5)) { }
		else if (strncmp(buff, "Connection:", 11)) { }
		else if (strncmp(buff, "User-Agent:", 11)) { }
		else if (strncmp(buff, "Accept:", 7)) { }
		else if (strncmp(buff, "Accept-Encoding:", 16)) { }
		else if (strncmp(buff, "Accept-Language:", 16)) { }
		else if (strncmp(buff, "Accept-Charset:", 15)) { }
		else {
			printf("httpd warning: unknown options '%s'", buff);
		}
	} while (res >= 0);

	/* Send answer */
	switch (ci.method) {
	default:
		break;
	case HTTP_METHOD_GET:
		http_method_get(&ci, buff, sizeof buff);
		break;
	}

	/* Close connection */
	close(sock);

	return;
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
