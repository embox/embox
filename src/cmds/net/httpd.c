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
#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#include <net/inetdevice.h>
#include <embox/cmd.h>
#include <ifaddrs.h>
#include <util/math.h>
#include <netinet/in.h>

EMBOX_CMD(httpd);


#define BUFF_SZ       (1460 * 4)
#define FILENAME_SZ   30
#define DEFAULT_PAGE  "index.html"


/* HTTP Methods */
enum http_method {
	HTTP_METHOD_UNKNOWN = 0,
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
};

/* Type of content */
enum http_content_type {
	HTTP_CONTENT_TYPE_HTML = 0,
	HTTP_CONTENT_TYPE_JPEG,
	HTTP_CONTENT_TYPE_PNG,
	HTTP_CONTENT_TYPE_GIF,
	HTTP_CONTENT_TYPE_ICO,
	HTTP_CONTENT_TYPE_UNKNOWN,
	HTTP_CONTENT_TYPE_MAX
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
	int sock;                      /* socket for client connection*/
	enum http_method method;       /* method in request */
	char file[FILENAME_SZ];        /* file to transmit */
	FILE *fp;                      /* descriptor of `file` */
	enum http_content_type c_type; /* type of contents which will be send */
	char buff[BUFF_SZ];            /* client's buffer (may contains more than one piece of data) */
	char *data;                    /* pointer to current chunk */
	char *next_data;               /* pointer to next piece of data in buffer */
	size_t next_len;               /* length of the next chunk */
};

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

/* Content type */
static const char *http_content_type_str[HTTP_CONTENT_TYPE_MAX] = {
	[HTTP_CONTENT_TYPE_HTML] = "text/html",
	[HTTP_CONTENT_TYPE_JPEG] = "image/jpeg",
	[HTTP_CONTENT_TYPE_PNG] = "image/png",
	[HTTP_CONTENT_TYPE_GIF] = "image/gif",
	[HTTP_CONTENT_TYPE_ICO] = "image/vnd.microsoft.icon",
	[HTTP_CONTENT_TYPE_UNKNOWN] = "application/unknown"
};

static char * get_next_line(struct client_info *info) {
	int res;
	size_t i, len;
	char *chunk;

	len = info->next_len;
	chunk = info->next_data;
	/* if no more data, return NULL */
	if ((chunk != NULL) && (len == 0)) {
		return NULL;
	}

	/* try to get next chunk (if we find end-line then exit) */
	for (i = 0; i < len; ++i) {
		if (chunk[i] == '\r') {
			chunk[i] = '\0';
			info->next_data = chunk + (i + 2);
			info->next_len = len - (i + 2);
			info->data = chunk;
			return chunk;
		}
	}
	/* need to recive more data (end-line not found): */
	/* 1. move next_chunk to head of buffer */
	chunk = memmove(info->buff, chunk, len);
	/* 2. get new piece if data */
	res = recv(info->sock, chunk + len, sizeof info->buff - len,
			0);
	if (res <= 0) {
		return NULL;
	}

	/* find chunks again */
	/* i == len there */
	for (len += res; i < len; ++i) {
		if (chunk[i] == '\r') {
			chunk[i] = '\0';
			info->next_data = chunk + (i + 2);
			info->next_len = len - (i + 2);
			info->data = chunk;
			return chunk;
		}
	}

	/* error: string is too long */
	printf("Error.. string is too long\n");
	info->next_len = 0;

	return NULL;
}

static int http_hnd_title(struct client_info *info) {
	char *method, *file, *param, *version;

//	printf("http_req_hdr: %s\n", info->data);

	method = info->data;

	file = strchr(method, ' ');
	if (file == NULL) {
		return HTTP_RET_ABORT; /* bad request */
	}

	*file = '\0', file = file + 2;

	if (strcmp(method, "GET") == 0) {
		info->method = HTTP_METHOD_GET;

		version = strchr(file, ' ');
		if (version == NULL) {
			return HTTP_RET_ABORT; /* bad request */
		}

		*version = '\0', version = version + 1;

		param = strchr(file, '?');
		if (param != NULL) {
			*param = '\0', param = param + 1;
		}

		if (strcmp(file, "") == 0) {
			file = DEFAULT_PAGE;
		}

		if (strlen(file) > sizeof info->file) {
			return HTTP_STAT_414;
		}
		strcpy(info->file, file);
	}
	else if (strcmp(method, "POST") == 0) {
		info->method = HTTP_METHOD_POST;
	}
	else {
		info->method = HTTP_METHOD_UNKNOWN;
		return HTTP_STAT_405; /* method unknown or unsupported */
	}

	return HTTP_RET_HNDOPS;
}

static int http_hnd_ops(struct client_info *info) {
	char *ops, *param;

//	printf("http_req_ops: %s\n", info->data);

	ops = info->data;
	if (strcmp(ops, "") == 0) {
		return HTTP_RET_ENDHDR;
	}

	param = strchr(ops, ':');
	if (param == NULL) {
		return HTTP_STAT_400;
	}

	*param = '\0', param = param + 2;

	if (strcmp(ops, "Host") == 0) { }
	else if (strcmp(ops, "Connection") == 0) { }
	else if (strcmp(ops, "User-Agent") == 0) { }
	else if (strcmp(ops, "Accept") == 0) { }
	else if (strcmp(ops, "Accept-Encoding") == 0) { }
	else if (strcmp(ops, "Accept-Language") == 0) { }
	else if (strcmp(ops, "Accept-Charset") == 0) { }
	else if (strcmp(ops, "Cache-Control") == 0) { }
	else if (strcmp(ops, "Referer") == 0) { }
	else {
		printf("httpd warning: unknown options: ops='%s' param='%s'\n", ops, param);
	}

	return HTTP_RET_OK;
}

static int process_request(struct client_info *info) {
	int res;
	int (*http_header_hnd)(struct client_info *);

	http_header_hnd = http_hnd_title; /* set first handler of data */
	while (get_next_line(info) != NULL) {
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

	return (http_header_hnd == http_hnd_title ? HTTP_RET_ABORT : HTTP_STAT_413);
}

static int http_req_get(struct client_info *info) {
	char *ext;

	info->fp = fopen(info->file, "r");
	if (info->fp == NULL) { /* file doesn't exist */
		return HTTP_STAT_404;
	}

	ext = strchr(info->file, '.');
	if (ext == NULL) {
		info->c_type = HTTP_CONTENT_TYPE_UNKNOWN;
	}
	else if ((strcmp(ext, ".htm") == 0)
			|| (strcmp(ext, ".html") == 0)) {
		info->c_type = HTTP_CONTENT_TYPE_HTML;
	}
	else if ((strcmp(ext, ".jpg") == 0)
			|| (strcmp(ext, ".jpeg") == 0)) {
		info->c_type = HTTP_CONTENT_TYPE_JPEG;
	}
	else if (strcmp(ext, ".png") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_PNG;
	}
	else if (strcmp(ext, ".gif") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_GIF;
	}
	else if (strcmp(ext, ".ico") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_ICO;
	}
	else {
		info->c_type = HTTP_CONTENT_TYPE_UNKNOWN;
	}

	return HTTP_STAT_200;
}

static int process_response(struct client_info *info) {
	switch (info->method) {
	default:
		return HTTP_STAT_405;
	case HTTP_METHOD_GET:
		return http_req_get(info);
	}
}

static void client_process(int sock, const struct sockaddr *addr) {
	int res;
	size_t bytes, bytes_need;
	struct client_info ci;
	int (*hnd)(struct client_info *);
	char *curr, buff[INET6_ADDRSTRLEN];

	memset(&ci, 0, sizeof ci);

	/* fill struct client_info */
	ci.sock = sock;

	hnd = process_request; /* request heandler for first */
process_again:
	res = hnd(&ci);
	assert((hnd == process_request) || (res != HTTP_RET_OK));
	switch (res) {
	default:
		printf("%s:%d -- upload %s ", inet_ntop(addr->sa_family,
					addr->sa_family == AF_INET
						? (void *)&((struct sockaddr_in *)addr)->sin_addr
						: (void *)&((struct sockaddr_in6 *)addr)->sin6_addr,
					buff, sizeof buff),
				addr->sa_family == AF_INET
					? ntohs(((struct sockaddr_in *)addr)->sin_port)
					: ntohs(((struct sockaddr_in6 *)addr)->sin6_port),
				ci.file);
		/* Make eader: */
		curr = ci.buff;
		/* 1. set title */
		assert((0 <= res) && (res < HTTP_STAT_MAX));
		curr += sprintf(curr, "HTTP/1.0 %s\r\n", http_stat_str[res]);
		/* 2. set ops */
		curr += sprintf(curr, "Content-Type: %s\r\n", http_content_type_str[ci.c_type]);
		curr += sprintf(curr, "Connection: %s\r\n", "close");
		curr += sprintf(curr, "\r\n");
		/* 3. set data and send respone */
		if (ci.fp == NULL) {
			/* send error */
			curr += sprintf(curr,
					"<html>"
					"<head><title>%s</title></head>"
					"<body><center><h1>Oops...</h1></center></body>"
					"</html>",
					http_stat_str[res]);
			bytes_need = curr - ci.buff;
			assert(bytes_need <= sizeof ci.buff); /* TODO remove this and make normal checks */
			bytes = send(ci.sock, ci.buff, bytes_need, 0);
			if (bytes != bytes_need) {
				perror("httpd: send() failure");
			}
		}
		else {
			/* send file */
			do {
				bytes_need = sizeof ci.buff - (curr - ci.buff);
				bytes = fread(curr, 1, bytes_need, ci.fp);
				if (bytes < 0) {
					break;
				}
				bytes_need = sizeof ci.buff - bytes_need + bytes;
				bytes = send(ci.sock, ci.buff, bytes_need, 0);
				if (bytes != bytes_need) {
					perror("httpd: send() failure");
					break;
				}
				curr = ci.buff;
				printf(".");
			} while (bytes_need == sizeof ci.buff);
		}
		printf(" done\n");
		break;
	case HTTP_RET_OK:
		hnd = process_response;
		goto process_again;
	case HTTP_RET_ABORT:
		break;
	}

	if (ci.fp != NULL) {
		fclose(ci.fp); /* close file (it's open or null) */
	}

	close(ci.sock); /* close connection */
}

static void welcome_message(void) {
	struct ifaddrs *ifa, *ifa_iter;
	int family;
	void *addr;
	char buff[max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];

	printf("Welcome to httpd!\n");

	if (-1 == getifaddrs(&ifa)) {
		perror("httpd: getifaddrs() failure");
		return;
	}

	for (ifa_iter = ifa; ifa_iter != NULL;
			ifa_iter = ifa_iter->ifa_next) {
		if (ifa_iter->ifa_addr == NULL) continue;

		family = ifa_iter->ifa_addr->sa_family;
		if ((family != AF_INET) && (family != AF_INET6)) continue;

		addr = family == AF_INET
				? (void *)&((struct sockaddr_in *)ifa_iter->ifa_addr)->sin_addr
				: (void *)&((struct sockaddr_in6 *)ifa_iter->ifa_addr)->sin6_addr;
		printf("\thttp://%s%s%s/\n",
				family == AF_INET6 ? "[" : "",
				inet_ntop(family, addr, buff, sizeof buff),
				family == AF_INET6 ? "]" : "");
	}

	freeifaddrs(ifa);
}

static int make_socket(int family, const struct sockaddr *addr,
		socklen_t addrlen) {
	int sock;

	sock = socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		perror("httpd: socket() failure");
		return -errno;
	}

	if (-1 == bind(sock, addr, addrlen)) {
		perror("httpd: bind() failure");
		close(sock);
		return -errno;
	}

	if (-1 == listen(sock, 3)) {
		perror("httpd: listen() failure");
		close(sock);
		return -errno;
	}

	return sock;
}

static int httpd(int argc, char **argv) {
	int host, client;
	union {
		struct sockaddr sa;
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
	} addr;
	socklen_t addrlen;

#if 1 /* IPv4 */
	addr.in.sin_family = AF_INET;
	addr.in.sin_port= htons(80);
	addr.in.sin_addr.s_addr = htonl(INADDR_ANY);

	host = make_socket(AF_INET, &addr.sa, sizeof addr.in);
	if (host < 0) {
		return host;
	}
#else /* IPv6 */
	addr.in6.sin6_family = AF_INET6;
	addr.in6.sin6_port= htons(80);
	memcpy(&addr.in6.sin6_addr, &in6addr_any,
			sizeof addr.in6.sin6_addr);

	host = make_socket(AF_INET6, &addr.sa, sizeof addr.in6);
	if (host < 0) {
		return host;
	}
#endif
	welcome_message();

	while (1) {
		addrlen  = sizeof addr;
		client = accept(host, &addr.sa, &addrlen);
		if (client == -1) {
			perror("httpd: accept() failure");
			continue;
		}
		if (((addr.sa.sa_family == AF_INET)
					&& (addrlen != sizeof addr.in))
				|| ((addr.sa.sa_family == AF_INET6)
					&& (addrlen != sizeof addr.in6))) {
			printf("httpd: bad addrlen %d for family %d\n",
					addrlen, addr.sa.sa_family);
			continue;
		}

		client_process(client, &addr.sa);
	}

	close(host);

	return 0;
}
