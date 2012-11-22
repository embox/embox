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
#include <arpa/inet.h>
#include <net/util/request_parser.h>
#include <net/in.h>
#include <lib/url_parser.h>
#include <embox/web_service.h>
#include <cmd/servd.h>

EMBOX_CMD(servd);

#define DEFAULT_PAGE  "index.html"
#define MAX_SERVICES_COUNT  10

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

struct params test_params;

//TODO it's work if buffer contains full starting line and headers
static int receive_and_parse_request(struct client_info *info) {
	int res;
	size_t len;
	char *chunk;
	char *prev;
	char *cur;

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

	/*parse starting line and headers*/
	prev = info->buff;
	cur = strchr(info->buff, '\n');
	while (cur - prev > 2) {
		cur++;
		prev = cur;
		cur = strchr(prev, '\n');
		if (cur == NULL) {

		}
	}
	cur[0] = '\0';

	info->parsed_request = parse_http(info->buff);

	//todo receive message body and set in some file

	return 1;
}

static int http_hnd_starting_line(struct client_info *info) {
//todo other check
	if (info->parsed_request == NULL) {
		return HTTP_RET_ABORT; /* bad request */
	}

	if (strcmp(info->parsed_request->method, "GET") == 0) {
		info->method = HTTP_METHOD_GET;

		if (info->parsed_request->proto == NULL) {
			return HTTP_RET_ABORT; /* bad request */
		}

		if (strlen(info->parsed_request->parsed_url->path)
				> sizeof info->file) {
			return HTTP_STAT_414;
		}

		if (strcmp(info->parsed_request->parsed_url->path, "") == 0) {
			strcpy(info->file, DEFAULT_PAGE);
		} else {
			strcpy(info->file, info->parsed_request->parsed_url->path);
		}

			test_params.info = info;
			test_params.query = info->parsed_request->parsed_url->query;
			event_init(&info->unlock_sock_event, "socket_lock");
			info->lock_status = 1;
			if (0 > web_service_send_message(info->file, &test_params)){
				info->lock_status = 0;
			}

	} else if (strcmp(info->parsed_request->method, "POST") == 0) {
		info->method = HTTP_METHOD_POST;
	} else {
		info->method = HTTP_METHOD_UNKNOWN;
		return HTTP_STAT_405; /* method unknown or unsupported */
	}

	return HTTP_RET_HNDOPS;
}

static int http_hnd_headers(struct client_info *info) {
	//todo process headers

	return HTTP_RET_OK;
}

static int process_request(struct client_info *info) {
	int res;

	while (receive_and_parse_request(info)) {
		res = http_hnd_starting_line(info);

		if (res != HTTP_RET_HNDOPS) {
			return res;
		}

		res = http_hnd_headers(info);

		if (res != HTTP_RET_OK) {
			return res;
		} else {
			return res;
		}
	}

	return HTTP_RET_ABORT;
}

static int http_req_get(struct client_info *info) {

	char *ext;

	if (info->lock_status) {
		event_wait(&info->unlock_sock_event, EVENT_TIMEOUT_INFINITE);
		info->lock_status = 0;
	}

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

static int set_starting_line(char *buff, int status_code) {
	return sprintf(buff, "HTTP/1.0 %s\r\n", http_stat_str[status_code]);
}

static int set_ops(char *buff, struct client_info *ci) {
	int res;
	res = sprintf(buff, "Content-Type: %s\r\n",
			http_content_type_str[ci->c_type]);
	res += sprintf(buff + res, "Connection: %s\r\n", "close");
	res += sprintf(buff + res, "\r\n");
	return res;
}

static void send_data(struct client_info *ci, int res) {
	char *curr;
	size_t bytes, bytes_need;

	/* Make header: */
	curr = ci->buff;
	/* 1. set title */
	assert((0 <= res) && (res < HTTP_STAT_MAX));
	curr += set_starting_line(curr, res);
	/* 2. set ops */
	curr += set_ops(curr, ci);
	/* 3. set mesaage bode and send respone */
	if (ci->fp == NULL) {
		/* send error */
		curr += sprintf(curr, "<html>"
				"<head><title>%s</title></head>"
				"<body><center><h1>Oops...</h1></center></body>"
				"</html>", http_stat_str[res]);
		bytes_need = curr - ci->buff;
		assert(bytes_need <= sizeof ci->buff); /* TODO remove this and make normal checks */
		bytes = sendto(ci->sock, ci->buff, bytes_need, 0, NULL, 0);
		if (bytes != bytes_need) {
			printf("http error: send() error\n");
		}
	} else {
		/* send file */
		do {
			bytes_need = sizeof ci->buff - (curr - ci->buff);
			bytes = fread(curr, 1, bytes_need, ci->fp);
			if (bytes < 0) {
				break;
			}
			bytes_need = sizeof ci->buff - bytes_need + bytes;
			bytes = sendto(ci->sock, ci->buff, bytes_need, 0, NULL, 0);
			if (bytes != bytes_need) {
				printf("http error: send() error\n");
				break;
			}
			curr = ci->buff;
			printf(".");
		} while (bytes_need == sizeof ci->buff);
	}
}

static void client_process(int sock, struct sockaddr_in addr,
		socklen_t addr_len) {
	int res;
	struct client_info ci;

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

	printf("%s:%d -- upload %s ", inet_ntoa(addr.sin_addr),
			ntohs(addr.sin_port), ci.file);
	send_data(&ci, res);
	printf(" done\n");

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
		new_task(start_server, NULL);
		//start_server(NULL);
	}

#if 0
	if (0 != thread_create(&thr, THREAD_FLAG_DETACHED, start_server, NULL)) {
		return -1;
	}
#endif

	return 0;
}
