/**
 * @file
 *
 * @date 03.12.12
 * @author Alexandr Chernakov
 * @author Vita Loginova
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

#include <cmd/servd.h>

#include <lib/service/service.h>

#define SERVICE_FILE_RAND_STR_LEN 10
#define SERVICE_FILE_PREFIX "/tmp/service-"
#define SERVICE_FILE_POSTFIX "-temp.html"

/* Status code */
const char *http_stat_str[HTTP_STAT_MAX] = {
		[HTTP_STAT_200] = "200 OK",
		[HTTP_STAT_400] = "400 Bad Request",
		[HTTP_STAT_404] = "404 Not Found",
		[HTTP_STAT_405] = "405 Method Not Allowed",
		[HTTP_STAT_408] = "408 Request Timeout", /* TODO */
		[HTTP_STAT_413] = "413 Request Entity Too Large",
		[HTTP_STAT_414] = "414 Request-URI Too Long",
		[HTTP_STAT_500] = "500 Internal Server Error"
};

/* Content type */
const char *http_content_type_str[HTTP_CONTENT_TYPE_MAX] = {
		[HTTP_CONTENT_TYPE_HTML]    = "text/html",
		[HTTP_CONTENT_TYPE_JPEG]    = "image/jpeg",
		[HTTP_CONTENT_TYPE_PNG]     = "image/png",
		[HTTP_CONTENT_TYPE_GIF]     = "image/gif",
		[HTTP_CONTENT_TYPE_ICO]     = "image/vnd.microsoft.icon",
		[HTTP_CONTENT_TYPE_UNKNOWN] = "application/unknown"
};

static const char *http_connection_str[] = {
		[HTTP_CONNECTION_CLOSE] = "close",
		[HTTP_CONNECTION_KEEP_ALIVE] = "keep-alive"
};

int get_content_type(const char *file_name) {
	char *ext;

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

static char rand_letter(void) {
	return 'a' + (rand() % ('z' - 'a' + 1));
}

int service_file_random_open_write(struct service_file *srv_file,
		const char *prefix, const char *postfix) {
	size_t i, prefix_len, rand_str_len, postfix_len;
	char *curr;

	prefix_len = strlen(prefix);
	rand_str_len = SERVICE_FILE_RAND_STR_LEN;
	postfix_len = strlen(postfix);

	srv_file->name = malloc(prefix_len + rand_str_len + postfix_len + 1);
	if (srv_file->name == NULL) {
		return -1;
	}

	strcpy(srv_file->name, prefix);
	for (curr = srv_file->name + prefix_len, i = rand_str_len; i != 0; --i) {
		*curr++ = rand_letter();
	}
	strcpy(curr, postfix);

	srv_file->fd = fopen(srv_file->name, "w");
	if (srv_file->fd == NULL) {
		service_file_close(srv_file);
		return -1;
	}

	return 0;
}

int service_file_open_write(struct service_file *srv_file) {
	return service_file_random_open_write(srv_file, SERVICE_FILE_PREFIX,
			SERVICE_FILE_POSTFIX);
}

int service_file_switch_to_read_mode(struct service_file *srv_file) {
	if (srv_file->fd != NULL) {
		fclose(srv_file->fd);
	} else {
		service_file_close(srv_file);
		return -1;
	}
	srv_file->fd = fopen(srv_file->name, "r");
	if (srv_file->fd == NULL) {
		service_file_close(srv_file);
		return -1;
	}
	return 0;
}

void service_file_close(struct service_file *srv_file) {
	if (srv_file->name != NULL) {
		remove(srv_file->name);
		free(srv_file->name);
	}
	if (srv_file->fd != NULL) {
		fclose(srv_file->fd);
	}
}

void service_free_service_data(struct service_data * data) {
	if (NULL != data) {
		free_http_request(&data->request);
		close(data->sock);
		free(data);
	}
}

void service_free_resourses(struct service_data *srv_data,
		struct service_file *srv_file) {
	service_file_close(srv_file);
	service_free_service_data(srv_data);
}

static int service_set_starting_line(char *buff, int status_code) {
	return sprintf(buff, "HTTP/1.0 %s\r\n", http_stat_str[status_code]);
}

static int service_set_ops(char *buff, size_t len, char *connection,
		int content_type) {
	int res, conn_type;

	conn_type = (strcmp(connection, http_connection_str[0]) == 0 ? 0 : 1);
	res = sprintf(buff, "Content-Type: %s\r\n",
			http_content_type_str[content_type]);
	res += sprintf(buff + res, "Content-Length: %d\r\n", (int)len);
	res += sprintf(buff + res, "Connection: %s\r\n",
			http_connection_str[conn_type]);
	res += sprintf(buff + res, "\r\n");
	return res;
}

int service_send_reply(struct service_data *srv_data,
		struct service_file *srv_file) {
	char buff[BUFF_SZ], *curr;
	int content_type;
	size_t bytes, bytes_need;
	struct stat stat;

	content_type = get_content_type(srv_file->name);
	/* Make header: */
	curr = buff;
	/* 1. set title */
	curr += service_set_starting_line(curr, srv_data->http_status);
	/* 2. set options */
	assert(srv_file->fd != NULL);
	fstat(fileno(srv_file->fd), &stat);
	curr += service_set_ops(curr, stat.st_size, srv_data->request.connection,
			content_type);
	/* 3. set message bode and send response */
	/* send file */
	do {
		bytes_need = sizeof buff - (curr - buff);
		bytes = fread(curr, 1, bytes_need, srv_file->fd);
		if (bytes < 0) {
			printf("http error: fread() error\n");
			return -1;
		}

		bytes_need = bytes + curr - buff;
		bytes = send(srv_data->sock, buff, bytes_need, 0);
		if (bytes != bytes_need) {
			printf("http error: send() error\n");
			return -1;
		}
		curr = buff;
	} while (bytes == sizeof buff);
	return 0;
}

//ToDo: create denying service
int service_send_error(struct service_data *srv_data,
		struct service_file *srv_file) {
	char *curr;
	int content_type;
	char msg[BUFF_SZ];
	size_t bytes, msg_len;
	const char *error_fmt = "<html>"
							"<head><title>%s</title></head>"
							"<body><center><h1>Oops...</h1></center></body>"
							"</html>";
	const size_t error_fmt_sz = strlen(error_fmt) - 2; /* i.e. without "%s" */

	content_type = get_content_type(srv_file->name);

	/* Make header: */
	curr = msg;
	/* 512 > sizeof error message */
	/* 1. set title */
	curr += service_set_starting_line(curr, HTTP_STAT_200);
	/* 2. set ops */
	curr += service_set_ops(curr, error_fmt_sz
				+ strlen(http_stat_str[srv_data->http_status]),
				srv_data->request.connection, content_type);
	/* 3. send error */
	curr += sprintf(curr, error_fmt, http_stat_str[srv_data->http_status]);

	msg_len = curr - msg;
	assert(msg_len <= sizeof msg); /* TODO remove this and make normal checks */
	bytes = send(srv_data->sock, msg, msg_len, 0);
	if (bytes != msg_len) {
		printf("http error: send() error\n");
	}
	return 1;
}
