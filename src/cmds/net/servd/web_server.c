/**
 * @file
 * @brief Simple HTTP server
 * @date 23.11.12
 * @author Vita Loginova
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/l3/ipv4/ip.h>

#include <embox/web_service.h>
#include <url_parser.h>
#include <lib/service/service.h>
#include <framework/mod/options.h>

#define DEFAULT_PAGE        OPTION_STRING_GET(default_page)
#define MAX_SERVICES_COUNT  OPTION_GET(number, max_services_amount)

static int get_method(const char *method) {
	if (strcmp(method, "GET") == 0) {
		return HTTP_METHOD_GET;
	}
	if (strcmp(method, "POST") == 0) {
		return HTTP_METHOD_POST;
	}
	return HTTP_METHOD_UNKNOWN; /* method unknown or unsupported */
}

//TODO it's work if buffer contains full starting line and headers
static int receive_and_parse_request(struct client_info *info) {
	int ret;
	char *chunk, *prev, *cur;
	size_t len;

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
	ret = recv(info->sock, chunk + len, sizeof info->buff - len, 0);
	if (ret <= 0){
		return 0;
	}

	/*parse starting line and headers*/
	prev = info->buff;
	cur = strchr(info->buff, '\n');
	assert(cur != NULL);
	while (cur - prev > 2) {
		cur++;
		prev = cur;
		cur = strchr(prev, '\n');
		assert(cur != NULL);
	}
	cur[0] = '\0';
	//todo receive message body and set in some file

	if (parse_http(info->buff, &info->parsed_request) < 0) {
		return 0;
	}

	return 1;
}

static int http_hnd_starting_line(struct client_info *info) {
//todo other check
	if (&info->parsed_request == NULL) {
		return HTTP_RET_ABORT; /* bad request */
	}

	info->method = get_method(info->parsed_request.method);
	if (info->method == HTTP_METHOD_GET) {
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
	} else if (info->method == HTTP_METHOD_POST) {
	} else {
		return HTTP_STAT_405; /* method unknown or unsupported */
	}

	return HTTP_RET_HNDOPS;
}

static int process_request(struct client_info *info) {
	int ret;

	if (receive_and_parse_request(info)) {
		ret = http_hnd_starting_line(info);
		if (ret != HTTP_RET_HNDOPS) {
			return ret;
		}
		return HTTP_RET_OK;
	}

	return HTTP_RET_ABORT;
}

static int http_req_get(struct client_info *info) {
	if (info->lock_status) {
		info->lock_status = 0;
	}

	if (info->fp == NULL) {
		info->fp = fopen(info->file, "r");
		if (info->fp == NULL) { /* file doesn't exist */
			return HTTP_STAT_404;
		}
	}

	info->c_type = get_content_type(info->file);

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

static void free_client_info(struct client_info *info) {
	if (info->fp != NULL) {
		fclose(info->fp); /* close file (it's open or null) */
	}
	close(info->sock); /* close connection */
	free_http_request(&info->parsed_request);
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
			struct service_data *srv_data;
			srv_data = malloc(sizeof(struct service_data));
			if (srv_data == NULL) {
				free_client_info(&ci);
				return;
			}
//
			//ToDo move it to web_service_start_service. now ci not needed to free
			srv_data->sock = ci.sock;
			memcpy(&srv_data->request, &ci.parsed_request, sizeof srv_data->request);
			srv_data->query = srv_data->request.parsed_url->query;

			if (web_service_start_service(ci.file, srv_data) < 0) {
				printf("client_process: start service error\n");
				service_free_service_data(srv_data);
			}
			return;
		} else {
			res = process_response(&ci);
			break;
		}
	case HTTP_RET_ABORT:
		free_client_info(&ci);
		return;
	}

	printf("-- upload %s ", ci.file);

	assert((0 <= res) && (res < HTTP_STAT_MAX));
	send_data(&ci, res);
	printf(" done\n");

	free_client_info(&ci);
}
