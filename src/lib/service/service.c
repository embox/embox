/**
 * @file
 *
 * @date Dec 3, 2012
 * @author: Alexandr Chernakov, Vita Loginova
 */

#include <lib/service/service.h>
#include <stdlib.h>
#include <string.h>
#include <cmd/servd.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define SERVICE_FILE_RAND_STR_LEN 10
#define SERVICE_FILE_PREFIX "/tmp/service-"
#define SERVICE_FILE_POSTFIX "-temp.html"

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

static const char *connection_types[] = {
		[HTPP_CONNECTION_CLOSE] = "close", [HTPP_CONNECTION_KEEP_ALIVE] = "keep-alive"};


static char rand_letter (void) {
	return 'a' + (rand() % 26);
}

int service_file_open_write(struct service_file *srv_file) {
	int i;
	int postfix_len = strlen(SERVICE_FILE_POSTFIX);
	int prefix_len = strlen(SERVICE_FILE_PREFIX);
	int f_name_len = prefix_len + SERVICE_FILE_RAND_STR_LEN + postfix_len + 1;

	srv_file->name = malloc(f_name_len);
	if (!srv_file->name) {
		return -1;
	}
	srv_file->name[0] = '\0';
	strcat (srv_file->name, SERVICE_FILE_PREFIX);
	for (i = strlen(SERVICE_FILE_PREFIX); i < SERVICE_FILE_RAND_STR_LEN + prefix_len; i++) {
		srv_file->name[i] = rand_letter();
	}
	srv_file->name[i] = '\0';
	strcat (srv_file->name, SERVICE_FILE_POSTFIX);
	srv_file->fd = fopen(srv_file->name, "w");
	if (!srv_file->fd) {
		service_file_close(srv_file);
		return -1;
	}

	return 0;
}

void service_get_service_data(struct service_data * data, void * arg) {
	*data = *((struct service_data *)arg);
}


int service_file_switch_to_read_mode(struct service_file *srv_file) {
	if (srv_file->fd) {
		fclose(srv_file->fd);
	} else {
		service_file_close(srv_file);
		return -1;
	}
	srv_file->fd = fopen(srv_file->name, "r");
	if (!srv_file->fd) {
		service_file_close(srv_file);
		return -1;
	}
	return 0;
}

void service_file_close(struct service_file *srv_file) {
	if (srv_file->name)
		free(srv_file->name);
	if (srv_file->fd)
		fclose(srv_file->fd);
}

static int service_set_starting_line(char *buff, int status_code) {
	return sprintf(buff, "HTTP/1.0 %s\r\n", http_stat_str[status_code]);
}

static int service_set_ops(char *buff, size_t len, int conn_type, int content_type) {
	int res;
	res = sprintf(buff, "Content-Type: %s\r\n",
			http_content_type_str[content_type]);
	res += sprintf(buff + res, "Content-Length: %d\r\n", len);
	res += sprintf(buff + res, "Connection: %s\r\n", connection_types[conn_type]);
	res += sprintf(buff + res, "\r\n");
	return res;
}

int service_send_reply(struct service_data *srv_data,
		struct service_file *srv_file) {
	char *curr;
	int content_type;
	char buff[BUFF_SZ];
	size_t bytes, bytes_need;
	struct stat stat;
	content_type = get_content_type(srv_file->name);
	/* Make header: */
	curr = buff;
	/* 1. set title */
	curr += service_set_starting_line(curr, srv_data->http_status);
	/* 2. set options */
	fstat(srv_file->fd->fd, &stat);
	curr += service_set_ops(curr, stat.st_size, HTPP_CONNECTION_CLOSE, content_type);
	/* 3. set message bode and send response */
	assert(srv_file->fd != NULL);
	/* send file */
	do {
		bytes_need = sizeof buff - (curr - buff);
		bytes = fread(curr, 1, bytes_need, srv_file->fd);
		if (bytes < 0) {
			break;

		}

		bytes_need = bytes + curr - buff;
		bytes = send(srv_data->sock, buff, bytes_need, 0);
		if (bytes != bytes_need) {
			printf("http error: send() error\n");
			break;
		}
		curr = buff;
	} while (bytes == bytes_need);
	return 1;
}

//ToDo: create denying service
int service_send_error(struct service_data *srv_data,
		struct service_file *srv_file) {
	char *curr;
	int content_type;
	char buff[BUFF_SZ];
	size_t bytes, bytes_need;
	const char *error_msg = "<html>"
			"<head><title></title></head>"
			"<body><center><h1>Oops...</h1></center></body>"
			"</html>";
	content_type = get_content_type(srv_file->name);

	/* Make header: */
	curr = buff;
	/* 512 > sizeof error message */
	/* 1. set title */
	curr += service_set_starting_line(curr, srv_data->http_status);
	/* 2. set ops */
	curr += service_set_ops(curr, strlen(error_msg) + strlen(http_stat_str[srv_data->http_status]),
			HTPP_CONNECTION_CLOSE, content_type);
	/* 3. send error */
	curr += sprintf(curr, "<html>"
				"<head><title>%s</title></head>"
				"<body><center><h1>Oops...</h1></center></body>"
				"</html>", http_stat_str[srv_data->http_status]);

	bytes_need = curr - buff;
	assert(bytes_need <= sizeof buff); /* TODO remove this and make normal checks */
	bytes = send(srv_data->sock, buff, bytes_need, 0);
	if (bytes != bytes_need) {
		printf("http error: send() error\n");
	}
	return 1;
}

void service_close_connection(struct service_data *srv_data) {
	close(srv_data->sock); /* close connection */
}

void service_free_service_data(struct service_data * data){
	if (NULL != data) {
			if (NULL != data->query) {
				free(data->query);
			}
			if (NULL != &data->request) {
				free_http_request(&data->request);
			}
			free(data);
		}
}
