/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol, Vita Loginova
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <net/ip.h>
#include <net/socket.h>
#include <embox/cmd.h>
#include <err.h>
#include <errno.h>
#include <net/inetdevice.h>
#include <net/in.h>
#include <net/util/request_parser.h>
#include <lib/url_parser.h>
#include <embox/web_service.h>

EMBOX_CMD(servd);

#define BUFF_SZ       (1460 * 2)
#define FILENAME_SZ   30
#define DEFAULT_PAGE  "index.html"

/* HTTP Methods */
enum http_method {
	HTTP_METHOD_UNKNOWN = 0, HTTP_METHOD_GET, HTTP_METHOD_POST
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
	HTTP_RET_ABORT, /* close connection */
	HTTP_RET_ENDHDR, /* end header section */
	HTTP_RET_HNDOPS /* set options handler */
};

struct client_info {
	int sock; /* socket for client connection*/
	enum http_method method; /* method in request */
	char file[FILENAME_SZ]; /* file to transmit */
	FILE *fp; /* descriptor of `file` */
	enum http_content_type c_type; /* type of contents which will be send */
	char buff[BUFF_SZ]; /* client's buffer (may contains more than one piece of data) */
	char *data; /* pointer to current chunk */
	char *next_data; /* pointer to next piece of data in buffer */
	size_t next_len; /* length of the next chunk */
	http_request * parsed; /* parsed request */
	struct event unlock_sock_event;
	int lock_status;
};

/* Status code */
static const char *http_stat_str[HTTP_STAT_MAX] = { [HTTP_STAT_200] = "200 OK",
		[HTTP_STAT_400] = "400 Bad Request", [HTTP_STAT_404] = "404 Not Found",
		[HTTP_STAT_405] = "405 Method Not Allowed", [HTTP_STAT_408
				] = "408 Request Timeout", /* TODO */
		[HTTP_STAT_413] = "413 Request Entity Too Large", [HTTP_STAT_414
				] = "414 Request-URI Too Long", };

/* Content type */
static const char *http_content_type_str[HTTP_CONTENT_TYPE_MAX] = {
		[HTTP_CONTENT_TYPE_HTML] = "text/html", [HTTP_CONTENT_TYPE_JPEG
				] = "image/jpeg", [HTTP_CONTENT_TYPE_PNG] = "image/png",
		[HTTP_CONTENT_TYPE_GIF] = "image/gif", [HTTP_CONTENT_TYPE_ICO
				] = "image/vnd.microsoft.icon", [HTTP_CONTENT_TYPE_UNKNOWN
				] = "application/unknown" };

struct params {
	struct client_info* info;
	char *text;
};

struct params test_params;

static const char *service_params[3][2] = { { DEFAULT_PAGE, "hello" }, {
		"about.html", "world" }, { "test.html", "world" }, };

static void start_services(void) {
	int i;

	for (i = 0; i < ARRAY_SIZE(service_params);	i++) {
		web_service_start(service_params[i][0]);
	}
}

static int is_service_started(char *name) {
	for (int i = 0; i < sizeof(service_params) / sizeof(service_params[0]);
			i++) {
		if (strcmp(service_params[i][0], name) == 0) {
			return 1;
		}
	}
	return 0;
}

static int is_lock_needed(char *name) {

	if (strcmp(service_params[2][0], name) == 0) {
		return 1;
	}
	return 0;
}

//TODO now it'll parse and fill struct
static int get_next_line(struct client_info *info) {
	int res;
	size_t len;
	char *chunk;

	len = info->next_len;
	chunk = info->next_data;
	/* if no more data, return NULL */
	if ((chunk != NULL) && (len == 0)) {
		return 0;
	}

	/* need to recive more data (end-line not found): */
	/* 1. move next_chunk to head of buffer */
	chunk = memmove(info->buff, chunk, len);
	/* 2. get new piece if data */
	res = recvfrom(info->sock, chunk + len, sizeof info->buff - len, 0, NULL,
			NULL);
	if (res <= 0) {
		return 0;
	}

	info->parsed = parse_http(info->buff);

	return 1;
}

static int http_hnd_title(struct client_info *info) {
//	printf("http_req_hdr: %s\n", info->data);
//todo other check
	if (info->parsed == NULL) {
		return HTTP_RET_ABORT; /* bad request */
	}

	if (strcmp(info->parsed->method, "GET") == 0) {
		info->method = HTTP_METHOD_GET;

		if (info->parsed->proto == NULL) {
			return HTTP_RET_ABORT; /* bad request */
		}

		if (strlen(info->parsed->parsed_url->path) > sizeof info->file) {
			return HTTP_STAT_414;
		}

		if (strcmp(info->parsed->parsed_url->path, "") == 0) {
			strcpy(info->file, DEFAULT_PAGE);
		} else {
			strcpy(info->file, info->parsed->parsed_url->path);
		}

		if (is_service_started(info->file)) {
			test_params.info = info;
			test_params.text = info->parsed->parsed_url->query;
			if (is_lock_needed(info->file)) {
				event_init(&info->unlock_sock_event, "socket_lock");
				info->lock_status = 1;
			}
			web_service_send_message(info->file, &test_params);
		}

	} else if (strcmp(info->parsed->method, "POST") == 0) {
		info->method = HTTP_METHOD_POST;
	} else {
		info->method = HTTP_METHOD_UNKNOWN;
		return HTTP_STAT_405; /* method unknown or unsupported */
	}

	return HTTP_RET_HNDOPS;
}
/*static int http_hnd_ops(struct client_info *info) {
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
 else if (strcmp(ops, "Cache-Control") == 0) { }static void *world(void *args) {
 printf("\n\n[error]\n\n");
 return NULL;
 }
 else if (strcmp(ops, "Referer") == 0) { }
 else {
 printf("httpd warning: unknown options: ops='%s' param='%s'\n", ops, param);
 }

 return HTTP_RET_OK;
 }*/

static int process_request(struct client_info *info) {
	int res;
	int (*http_header_hnd)(struct client_info *);

	http_header_hnd = http_hnd_title; /* set first handler of data */
	while (get_next_line(info)) {
		res = http_header_hnd(info);
		switch (res) {
		default:
			return res;
		case HTTP_RET_OK: /* all ok, continue */
			break;
		case HTTP_RET_HNDOPS: /* set option's handler */
			/*http_header_hnd = http_hnd_ops;
			 break;*/
			return HTTP_RET_OK;
		case HTTP_RET_ENDHDR: /* end header section */
			return HTTP_RET_OK;
		}
	}

	return (http_header_hnd == http_hnd_title ? HTTP_RET_ABORT : HTTP_STAT_413);
}

static int http_req_get(struct client_info *info) {

	char *ext;

	if (info->fp == NULL) {
		info->fp = fopen(info->file, "r");
		if (info->fp == NULL) { /* file doesn't exist */
			return HTTP_STAT_404;
		}
	}

	ext = strchr(info->file, '.');
	if (ext == NULL) {
		info->c_type = HTTP_CONTENT_TYPE_UNKNOWN;
	} else if ((strcmp(ext, ".htm") == 0) || (strcmp(ext, ".html") == 0)) {
		info->c_type = HTTP_CONTENT_TYPE_HTML;
	} else if ((strcmp(ext, ".jpg") == 0) || (strcmp(ext, ".jpeg") == 0)) {
		info->c_type = HTTP_CONTENT_TYPE_JPEG;
	} else if (strcmp(ext, ".png") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_PNG;
	} else if (strcmp(ext, ".gif") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_GIF;
	} else if (strcmp(ext, ".ico") == 0) {
		info->c_type = HTTP_CONTENT_TYPE_ICO;
	} else {
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

static void client_process(int sock, struct sockaddr_in addr,
		socklen_t addr_len) {
	int res;
	size_t bytes, bytes_need;
	struct client_info ci;
	char *curr;

	memset(&ci, 0, sizeof ci);

	/* fill struct client_info */
	ci.sock = sock;


	/* request heandler for first */
	res = process_request(&ci);
	switch (res) {
	case HTTP_RET_OK:
		res = process_response(&ci);
		break;
	case HTTP_RET_ABORT:
		/*close and exit*/
		break;
	}

	switch (res) {
	default:
		if (ci.lock_status) {
			event_wait(&ci.unlock_sock_event, EVENT_TIMEOUT_INFINITE);
		}
		printf("%s:%d -- upload %s ", inet_ntoa(addr.sin_addr),
				ntohs(addr.sin_port), ci.file);
		/* Make header: */
		curr = ci.buff;
		/* 1. set title */
		assert((0 <= res) && (res < HTTP_STAT_MAX));
		curr += sprintf(curr, "HTTP/1.0 %s\r\n", http_stat_str[res]);
		/* 2. set ops */
		curr += sprintf(curr, "Content-Type: %s\r\n",
				http_content_type_str[ci.c_type]);
		curr += sprintf(curr, "Connection: %s\r\n", "close");
		curr += sprintf(curr, "\r\n");
		/* 3. set data and send respone */
		if (ci.fp == NULL) {
			/* send error */
			curr += sprintf(curr, "<html>"
					"<head><title>%s</title></head>"
					"<body><center><h1>Oops...</h1></center></body>"
					"</html>", http_stat_str[res]);
			bytes_need = curr - ci.buff;
			assert(bytes_need <= sizeof ci.buff); /* TODO remove this and make normal checks */
			bytes = sendto(ci.sock, ci.buff, bytes_need, 0, NULL, 0);
			if (bytes != bytes_need) {
				printf("http error: send() error\n");
			}
		} else {
			/* send file */
			do {
				bytes_need = sizeof ci.buff - (curr - ci.buff);
				bytes = fread(curr, 1, bytes_need, ci.fp);
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
				printf(".");
			} while (bytes_need == sizeof ci.buff);
		}
		printf(" done\n");
		break;
	}

	if (ci.fp != NULL) {
		fclose(ci.fp); /* close file (it's open or null) */
	}

	close(ci.sock); /* close connection */
}

static void welcome_message(void) {
	/* FIXME cheat to get local ip */
	struct in_addr localAddr;
	struct in_device *in_dev = inet_dev_find_by_name("eth0");
	localAddr.s_addr = in_dev->ifa_address;
	printf("Welcome to http://%s\n", inet_ntoa(localAddr));
}

static void *start_server(void* args) {
	int res, host;
	socklen_t addr_len;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		return (void*) host;
	}

	res = bind(host, (struct sockaddr *) &addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		return (void*) res;
	}

	res = listen(host, 1);
	if (res < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		return (void*) res;
	}

	welcome_message();

	start_services();

	while (1) {
		res = accept(host, (struct sockaddr *) &addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("Error.. accept() failed. errno=%d\n", errno);
			close(host);
			return (void*) res;
		}
		client_process(res, addr, addr_len);
	}

	close(host);
	return (void*) 0;
}

#include <kernel/task.h>

static int web_server_started;

static int servd(int argc, char **argv) {
	if (0 == web_server_started) {
		new_task(start_server, NULL, 0);
		//start_server(NULL);
	}

#if 0
	if (0 != thread_create(&thr, THREAD_FLAG_DETACHED, start_server, NULL)) {
		return -1;
	}
#endif

	return 0;
}
