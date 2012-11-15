/**
 * @file
 *
 * @date Nov 8, 2012
 * @author: Vita Loginova
 */

#include <embox/web_service.h>
#include <stdio.h>
#include <string.h>
#include <net/util/request_parser.h>

#define BUFF_SZ       (1460 * 2)
#define FILENAME_SZ   30

enum http_method {
	HTTP_METHOD_UNKNOWN = 0, HTTP_METHOD_GET, HTTP_METHOD_POST
};

enum http_content_type {
	HTTP_CONTENT_TYPE_HTML = 0,
	HTTP_CONTENT_TYPE_JPEG,
	HTTP_CONTENT_TYPE_PNG,
	HTTP_CONTENT_TYPE_GIF,
	HTTP_CONTENT_TYPE_ICO,
	HTTP_CONTENT_TYPE_UNKNOWN,
	HTTP_CONTENT_TYPE_MAX
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

struct params {
	struct client_info* info;
	char *text;
};

static void *process_params(void* args) {
	struct web_service_instance *inst;
	struct params *params;
	char buff[512];
	FILE *file;
	int n;
	int param_flag = 0;

	inst = (struct web_service_instance *) args;
	params = (struct params *) inst->params;

	file = fopen(params->info->file, "r");
	if (file == NULL) { /* file doesn't exist */
		return NULL;
	}
	params->info->fp = fopen("/tmp/test_temp.html", "w");
	if (params->info->fp == NULL) { /* file doesn't exist */
		fclose(file);
		return NULL;
	}

	while ((n = fread(buff, sizeof(char), sizeof buff, file)) > 0) {
		char *param;

		if (param_flag) {
			if ((param = strchr(buff, '}')) != NULL) {
				param++;
				fwrite(param, sizeof(char), strlen(param), params->info->fp);
				param_flag = 0;
			} else {
				continue;
			}
		}

		if ((param = strchr(buff, '$')) != NULL) {
			fwrite(buff, sizeof(char), param - buff, params->info->fp);
			if (params->text != NULL) {
				fwrite(params->text, sizeof(char), strlen(params->text),
						params->info->fp);
			}
			if ((param = strchr(param, '}')) != NULL) {
				param++;
				fwrite(param, sizeof(char), strlen(param), params->info->fp);
			} else {
				param_flag = 1;
			}
		} else {
			fwrite(buff, sizeof(char), n, params->info->fp);
		}
	}

	fclose(file);
	fclose(params->info->fp);


	params->info->lock_status = 0;
	event_notify(&params->info->unlock_sock_event);
	params->info->fp = fopen("/tmp/test_temp.html", "r");

	return NULL;

}

EMBOX_WEB_SERVICE("test.html", process_params);
