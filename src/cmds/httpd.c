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
	FILE *xmit_file;          /* file to transmit */
	char *xmit_err;           /* error (used if file_xmit is null) */
	char buff[BUFF_SZ];       /* client's buffer (may contains more than one piece of data) */
	char *data;               /* pointer to current chunk */
	char *next_data;          /* pointer to next piece of data in buffer */
	size_t next_len;          /* length of the next chunk */
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
	res = recvfrom(info->sock, chunk + len, sizeof info->buff - len, 0, NULL, NULL);
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

	printf("http_req_hdr: %s\n", info->data);

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
		info->xmit_file = fopen(file, "r");
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

	printf("http_req_ops: %s\n", info->data);

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
	else {
		printf("httpd warning: unknown options: ops='%s' param='%s'\n", ops, param);
	}

	return HTTP_RET_OK;
}

static int process_request(struct client_info *info) {
	int res;
	int (*http_header_hnd)(struct client_info *);

	http_header_hnd = http_hnd_title; /* set first heandler of data */
	while (get_next_line(info) != NULL) {
		res = http_header_hnd(info);
		printf("http_header_hnd=%d\n", res);
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

	printf("failed\n");
	return (http_header_hnd == http_hnd_title ? HTTP_STAT_414 : HTTP_STAT_413);
}

static int http_req_get(struct client_info *info) {
	if (info->xmit_file == NULL) { /* file doesn't exist */
		info->xmit_err = "<html>\
						  <head><title>404</title></head>\
						  <body><center><h1>Oops...</h1></center></body>\
						  </html>";
		return HTTP_STAT_404;
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

static void client_process(int sock) {
	int res;
	size_t bytes, bytes_need;
	struct client_info ci;
	int (*hnd)(struct client_info *);
	char *curr;

	memset(&ci, 0, sizeof ci);

	/* fill struct client_info */
	ci.sock = sock;

	hnd = process_request; /* request heandler for first */
process_again:
	res = hnd(&ci);
	assert((hnd == process_request) || (res != HTTP_RET_OK));
	switch (res) {
	default:
		/* Make header: */
		curr = ci.buff;
		/* 1. set title */
		assert((0 <= res) && (res < HTTP_STAT_MAX));
		curr += sprintf(curr, "HTTP/1.0 %s\r\n", http_stat_str[res]);
		/* 2. set ops */
		curr += sprintf(curr, "Content-Type: %s\r\n", "text/html");
		curr += sprintf(curr, "Connection: %s\r\n", "close");
		curr += sprintf(curr, "\r\n");
		/* 3. set data and send respone */
		if (ci.xmit_file == NULL) {
			/* send error */
			assert(ci.xmit_err != NULL);
			curr += sprintf(curr, "%s", ci.xmit_err);
			bytes_need = curr - ci.buff;
			assert(bytes_need <= sizeof ci.buff); /* TODO remove this and make normal checks */
			bytes = sendto(ci.sock, ci.buff, bytes_need, 0, NULL, 0);
			if (bytes != bytes_need) {
				printf("http error: send() error\n");
			}
		}
		else {
			/* send file */
			do {
				bytes_need = sizeof ci.buff - (curr - ci.buff);
				bytes = fread(curr, 1, bytes_need, ci.xmit_file);
				if (bytes < 0) {
					break;
				}
				bytes_need = sizeof ci.buff - bytes_need + bytes;
				bytes = sendto(ci.sock, ci.buff, bytes_need, 0, NULL, 0);
				if (bytes != bytes_need) {
					printf("http error: send() error\n");
					break;
				}
				curr = ci.buff;
			} while (bytes_need == sizeof ci.buff);
		}
		break;
	case HTTP_RET_OK:
		hnd = process_response;
		goto process_again;
	case HTTP_RET_ABORT:
		break;
	}

	if (ci.xmit_file != NULL) {
		fclose(ci.xmit_file); /* close file (it's open or null) */
	}

	close(ci.sock); /* close connection */
}

static int exec(int argc, char **argv) {
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
