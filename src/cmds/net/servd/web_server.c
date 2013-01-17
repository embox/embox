/**
 * @file
 * @brief Simple HTTP server
 * @date 23.11.12
 * @author Vita Loginova
 */

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <net/ip.h>

#include <embox/web_service.h>
#include <lib/url_parser.h>
#include <lib/service/service.h>


#define DEFAULT_PAGE  "index.html"
#define MAX_SERVICES_COUNT  10

struct params test_params;

/* Status code */
const char *http_stat_str[HTTP_STAT_MAX] = { [HTTP_STAT_200] = "200 OK",
		[HTTP_STAT_400] = "400 Bad Request", [HTTP_STAT_404] = "404 Not Found",
		[HTTP_STAT_405] = "405 Method Not Allowed", [HTTP_STAT_408
				] = "408 Request Timeout", /* TODO */
		[HTTP_STAT_413] = "413 Request Entity Too Large", [HTTP_STAT_414
				] = "414 Request-URI Too Long", [HTTP_STAT_500] = "500 Internal Server Error"};

/* Content type */
const char *http_content_type_str[HTTP_CONTENT_TYPE_MAX] = {
		[HTTP_CONTENT_TYPE_HTML] = "text/html", [HTTP_CONTENT_TYPE_JPEG
				] = "image/jpeg", [HTTP_CONTENT_TYPE_PNG] = "image/png",
		[HTTP_CONTENT_TYPE_GIF] = "image/gif", [HTTP_CONTENT_TYPE_ICO
				] = "image/vnd.microsoft.icon", [HTTP_CONTENT_TYPE_UNKNOWN
				] = "application/unknown" };

void free_client_info(struct client_info * info) {
	free_http_request(&info->parsed_request);
}

int get_content_type(char *file_name) {
	char* ext;
	ext = strchr(file_name, '.');
	if (ext == NULL) {
		return HTTP_CONTENT_TYPE_UNKNOWN;
	}
	if ((strcmp(ext, ".htm") == 0) || (strcmp(ext, ".html") == 0)) {
		return HTTP_CONTENT_TYPE_HTML;
	}
	if ((strcmp(ext, ".jpg") == 0) || (strcmp(ext, ".jpeg") == 0)) {
		return HTTP_CONTENT_TYPE_JPEG;
	}
	if (strcmp(ext, ".png") == 0) {
		return HTTP_CONTENT_TYPE_PNG;
	}
	if (strcmp(ext, ".gif") == 0) {
		return HTTP_CONTENT_TYPE_GIF;
	}
	if (strcmp(ext, ".ico") == 0) {
		return HTTP_CONTENT_TYPE_ICO;
	}
	return HTTP_CONTENT_TYPE_UNKNOWN;
}

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

	res = recv(info->sock, chunk + len, sizeof info->buff - len, 0);

	if(res < 0){
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
	//todo receive message body and set in some file

	if (0 > parse_http(info->buff, &info->parsed_request)) {
		return 0;
	}

	return 1;
}

static int http_hnd_starting_line(struct client_info *info) {
//todo other check
	if (&info->parsed_request == NULL) {
		return HTTP_RET_ABORT; /* bad request */
	}

	if (strcmp(info->parsed_request.method, "GET") == 0) {
		info->method = HTTP_METHOD_GET;

		if (info->parsed_request.proto == NULL) {
			return HTTP_RET_ABORT; /* bad request */
		}

		if (strlen(info->parsed_request.parsed_url->path) > sizeof info->file) {
			return HTTP_STAT_414;
		}

		if (strcmp(info->parsed_request.parsed_url->path, "") == 0) {
			strcpy(info->file, DEFAULT_PAGE);
		} else {
			strcpy(info->file, info->parsed_request.parsed_url->path);
		}

		test_params.info = info;
		test_params.query = info->parsed_request.parsed_url->query;
	} else if (strcmp(info->parsed_request.method, "POST") == 0) {
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

static void send_data(struct client_info *ci, int stat) {
	char *curr;
	size_t bytes, bytes_need;

	/* Make header: */
	curr = ci->buff;
	/* 1. set title */
	curr += set_starting_line(curr, stat);
	/* 2. set ops */
	curr += set_ops(curr, ci);
	/* 3. set mesaage bode and send respone */
	if (ci->fp == NULL) {
		/* send error */
		curr += sprintf(curr, "<html>"
				"<head><title>%s</title></head>"
				"<body><center><h1>Oops...</h1></center></body>"
				"</html>", http_stat_str[stat]);
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
		} while (bytes_need == sizeof ci->buff);
	}
}

void close_connection(struct client_info *ci) {
	if (ci->fp != NULL) {
		fclose(ci->fp); /* close file (it's open or null) */
	}
	close(ci->sock); /* close connection */
	free_client_info(ci);
}

void client_process(int sock) {
	int res;
	struct client_info ci;

	memset(&ci, 0, sizeof ci);

	/* fill struct client_info */
	ci.sock = sock;
	/* request handler for first */
	res = process_request(&ci);
	// Get rid of static pages and services that is not started
	// Others have to be dispatched to responding service instance
	// according to serv_desc table
	switch (res) {
	case HTTP_RET_OK:
		//Start the responding service instance thread

		if (is_service_started(ci.file)) {
			struct service_data* srv_data = malloc(sizeof(struct service_data));
			if (srv_data == NULL) {
				printf("client_process: malloc failed\n");
				return;
			}

			//ToDo move it to web_service_start_service
			srv_data->sock = ci.sock;

			if (request_parser_copy(&srv_data->request, &ci.parsed_request) < 0) {
				printf("client_process: request_parser_copy failed\n");
				return;
			}

			srv_data->query = srv_data->request.parsed_url->query;

			if (web_service_start_service(ci.file, srv_data) < 0) {
				printf("client_process: start service error\n");
			}
			return;
		} else {
			res = process_response(&ci);
			break;
		}
	case HTTP_RET_ABORT:
		close_connection(&ci);
		return;
	}

	printf("-- upload %s ", ci.file);

	assert((0 <= res) && (res < HTTP_STAT_MAX));
	send_data(&ci, res);
	printf(" done\n");

	close_connection(&ci);
}

