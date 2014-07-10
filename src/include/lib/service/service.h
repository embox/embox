/**
 * @file
 *
 * @date Dec 3, 2012
 * @author: Alexandr Chernakov, Vita Loginova
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <net/util/request_parser.h>
#include <stdio.h>
#include <cmd/servd.h>

struct service_data {
	char *query;
	int sock;
	int http_status;
	struct http_request request;
};

struct service_file {
	FILE *fd;
	char *name;
};

/* Status code */
extern const char *http_stat_str[HTTP_STAT_MAX];
/* Content type */
extern const char *http_content_type_str[HTTP_CONTENT_TYPE_MAX];

extern int service_file_open_write(struct service_file *srv_file);

extern int service_file_switch_to_read_mode(struct service_file *srv_file);

extern int service_send_error(struct service_data *srv_data,
		struct service_file *srv_file);

extern void service_file_close(struct service_file *srv_file);

extern int service_send_reply(struct service_data *srv_data, struct service_file *srv_file);

extern void service_free_service_data(struct service_data * data);

extern void service_free_resourses(struct service_data *srv_data, struct service_file *srv_file);

extern int get_content_type(const char *file_name);

#endif /* SERVICE_H_ */
