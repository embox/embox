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

#define MAX_FNAME     32
#define BUFF_SZ       512
#define DEFAULT_PAGE  "index.html"

/* HTTP Methods */
enum http_method {
	HTTP_METHOD_UNKNOWN = 0,
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
};

/* Returns code */
enum http_ret {
	/* HTTP status code */
	/* 1xx Informational */
	/* 2xx Success */
	HTTP_STAT_200 = 0,
	/* 3xx Redirection */
	/* 4xx Client Error */
	HTTP_STAT_400,
	HTTP_STAT_404,
	HTTP_STAT_405,
	HTTP_STAT_408,
	HTTP_STAT_413,
	HTTP_STAT_414,
	/* 5xx Server Error */
	HTTP_STAT_MAX, /* for implementation */
	/* Other code */
	HTTP_RET_OK = -100, /* ok */
	HTTP_RET_ABORT,     /* close connection */
	HTTP_RET_ENDHDR,    /* end header section */
	HTTP_RET_HNDOPS     /* set options handler */
};

struct client_info {
	int sock;                 /* socket for client connection*/
	struct sockaddr_in addr;  /* client address */
	socklen_t addr_len;       /* address length */
	enum http_method method;  /* method in request */
	char req_file[MAX_FNAME]; /* client want open this page */
	char buff[BUFF_SZ];       /* client's buffer (may contains more than one piece of data) */
	char *data;               /* pointer to current chunk */
	size_t len;               /* size of data */
	char *next_data;          /* pointer to next piece of data in buffer */
	size_t next_len;          /* length of the next chunk */
};

/* Methods table */
#define METHOD_INFO_SET(name) {(#name " "), sizeof (#name " "), HTTP_METHOD_##name }
const struct { char *n; size_t l; enum http_method m; } methods_info[] = {
	METHOD_INFO_SET(GET), METHOD_INFO_SET(POST)
};
#undef METHOD_INFO_SET

/* Status code */
static const char *http_stat_str[HTTP_STAT_MAX] = {
	[HTTP_STAT_200] = "200 OK",
	[HTTP_STAT_400] = "400 Bad Request",
	[HTTP_STAT_404] = "404 Not Found",
	[HTTP_STAT_405] = "405 Method Not Allowed",
	[HTTP_STAT_408] = "408 Request Timeout", /* TODO */
	[HTTP_STAT_413] = "413 Request Entity Too Large",
	[HTTP_STAT_414] = "414 Request-URI Too Long",
};

static int get_next_chunk(struct client_info *info) {
	int res;
	size_t i, len;
	char *chunk;

	len = info->next_len;
	chunk = info->next_data;
	/* if no more data, return NULL */
	if (len == 0) {
		info->len = 0;
		return 0;
	}

	/* try to get next chunk (if we find end-line then exit) */
	for (i = 0; i < len; ++i) {
		if (chunk[i] == '\n') {
			/* chunk[i] = '\0'; */
			info->next_data = chunk + (i + 1);
			info->next_len = len - (i + 1);
			info->data = chunk;
			info->len = (i + 1);
			return 0;
		}
	}
	/* need to recive more data (end-line not found): */
	/* 1. move next_chunk to head of buffer */
	chunk = memmove(info->buff, chunk, len);
	/* 2. get new piece if data */
	res = recvfrom(info->sock, chunk, sizeof info->buff - len, 0,
			(struct sockaddr *)&info->addr, &info->addr_len);
	if (res <= 0) {
		/* chunk[len] = '\0'; */
		info->next_len = 0;
		info->data = chunk;
		info->len = len;
		return 0;
	}

	/* find chunks again */
	/* i == len there */
	for (len += res; i < len; ++i) {
		if (chunk[i] == '\n') {
			/* chunk[i] = '\0'; */
			info->next_data = chunk + (i + 1);
			info->next_len = len - (i + 1);
			info->data = chunk;
			info->len = (i + 1);
			return 0;
		}
	}

	/* error: string is too long */
	printf("Error.. string is too long\n");
	info->next_len = 0;
	info->data = chunk;
	info->len = len;

	return -1;
}

static enum http_method parse_method(char *buff, size_t len) {
	size_t i;

	for (i = 0; i < sizeof methods_info / sizeof methods_info[0]; ++i) {
		if ((len >= methods_info[i].l)
				&& (memcmp(buff, methods_info[i].n, methods_info[i].l) == 0)) {
			return methods_info[i].m;
		}
	}

	return HTTP_METHOD_UNKNOWN;
}

static int http_hnd_title(struct client_info *info) {
	size_t i, len;
	char *data;

	printf("http_req_hdr: ");
	write(1, info->data, info->len);

	len = info->len;
	data = info->data;
	switch ((info->method = parse_method(data, len))) {
	default: /*  unknown or unsupported */
		return HTTP_STAT_405;
	case HTTP_METHOD_GET:
		/* parse file */
		for (i = 4; i < len; ++i) {
			if ((data[i] == ' ') || (data[i] == '?')) {
				if (strncmp(data + 4, "/ ", 2) == 0) {
					strcpy(info->req_file, DEFAULT_PAGE);
				}
				else {
					strncpy(info->req_file, data + 5, i - 5);
				}
				return HTTP_RET_HNDOPS;
			}
		}
		return HTTP_STAT_414;
	}
}

static int http_hnd_ops(struct client_info *info) {
	size_t i, len;
	char opt[32], *data;

	data = info->data;
	len = info->len;

	for (i = 0; i < len; ++i) {
		if ((data[i] == ':') && (data[i] == '\r')) {
			strncpy(opt, data, i);
			break;
		}
	}
	if (i == len) {
		return HTTP_STAT_400;
	}

	printf("http_req_ops: ");
	write(1, info->data, info->len);

	if (strcmp(data, "") == 0) {
		return HTTP_RET_ENDHDR;
	}
	else if (strcmp(data, "Host") == 0) { }
	else if (strcmp(data, "Connection") == 0) { }
	else if (strcmp(data, "User-Agent") == 0) { }
	else if (strcmp(data, "Accept") == 0) { }
	else if (strcmp(data, "Accept-Encoding") == 0) { }
	else if (strcmp(data, "Accept-Language") == 0) { }
	else if (strcmp(data, "Accept-Charset") == 0) { }
	else {
		printf("httpd warning: unknown options");
	}

	return HTTP_RET_OK;
}

static int process_request(struct client_info *info) {
	int res;
	int (*http_header_hnd)(struct client_info *);

	http_header_hnd = http_hnd_title; /* set first heandler of data */
	while (get_next_chunk(info) == -1) {
		res = http_header_hnd(info);
		switch (res) {
		default:
			return res;
		case HTTP_RET_OK: /* all ok, continue */
			break;
		case HTTP_RET_HNDOPS: /* set option's handler */
			http_header_hnd = http_hnd_ops;
			break;
		case HTTP_RET_ENDHDR: /* end header section */
			return HTTP_RET_OK;
		}
	}

	return HTTP_STAT_413;
}

static int http_req_get(struct client_info *info) {
	FILE *fp;

	fp = fopen(info->req_file, "r");
	if (fp == NULL) {
		return HTTP_STAT_404;
	}
	fclose(fp);
	return HTTP_STAT_200;
#if 0
	if (fd < 0) {
		curr += sprintf(curr,
				"HTTP/1.0 200 OK\n"
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
		printf("Send to host: %s\n", buff);
		send(info->sock, buff, curr - buff, 0);
		return -1;
	}

	curr += sprintf(curr, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
	do {
		bytes_read = len - (curr - buff);
		bytes = read(fd, curr, bytes_read);
		if (bytes < 0) {
			close(fd);
			return -1;
		}
		curr += bytes;
		printf("Send to host: %s\n", buff);
		if (send(info->sock, buff, curr - buff, 0) < 0) {
			printf("Error.. send failed\n");
			close(fd);
			return -1;
		}
		curr = buff;
	} while (bytes == bytes_read);

	close(fd);

	return 0;
#endif
}

static int process_response(struct client_info *info) {
	switch (info->method) {
	default:
		assert(0);
		return HTTP_STAT_405;
	case HTTP_METHOD_GET:
		return http_req_get(info);
	}
}

static void client_process(int sock) {
	int res;
	struct client_info ci;
	int (*hnd)(struct client_info *);
	char *curr;

	memset(&ci, 0, sizeof ci);

	/* fill struct client_info */
	ci.sock = sock;

	hnd = process_request; /* request heandler for first */
process_again:
	res = hnd(&ci);
	switch (res) {
	default:
		/* Make header: */
		curr = ci.buff;
		/* 1. set title */
		curr += sprintf(curr, "HTTP/1.0 %s\r\n", http_stat_str[res]);
		/* 2. set ops */
		curr += sprintf(curr, "Content-Type: %s\r\n", "text/html");
		/* Send file */
		printf("RESPONSE:\n %s", ci.buff);
		break;
	case HTTP_RET_OK:
		hnd = process_response;
		goto process_again;
	case HTTP_RET_ABORT:
		break;
	}

	/* close connection */
	close(sock);

	return;
}

int exec(int argc, char **argv) {
	int res, host;
	socklen_t addr_len;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port= htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host < 0) {
		printf("Error.. can't create socket\n");
		return host;
	}

	res = bind(host, (struct sockaddr *)&addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed\n");
		return res;
	}

	res = listen(host, 1);
	if (res < 0) {
		printf("Error.. listen() failed\n");
		return res;
	}

	while (1) {
		res = accept(host,(struct sockaddr *)&addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("Error.. accept() failed");
			close(host);
			return res;
		}
		client_process(res);
	}

	close(host);

	return 0;
}
